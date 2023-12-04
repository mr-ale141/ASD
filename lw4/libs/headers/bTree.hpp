#pragma once
#include <iostream>
#include <fstream>
#include <variant>
#include <stdio.h>

typedef unsigned long long indexType;

template <typename recordType, typename keyType>
class Node
{
public:
    bool isLeaf;
    int N;
    int size;
    indexType parent;
    indexType current;
    keyType *keys;
    indexType *children;
    recordType* data;

    explicit Node(int treeN)
    {
        isLeaf = false;
        N = treeN;
        size = 0;
        parent = 0;
        current = 0;

        auto *_keys = new keyType[2 * N - 1];
        for (int i = 0; i < (2 * N - 1); i++)
            _keys[i] = 0;
        keys = _keys;

        auto *_children = new indexType[2 * N];
        for (int i = 0; i < (2 * N); i++)
            _children[i] = 0;
        children = _children;

        auto *_data = new recordType[2 * N - 1];
        data = _data;
    }
};

template <typename recordType, typename keyType>
class BTree
{
    int N;
    indexType rootIndex;
    indexType writeIndex;
    std::size_t sizeBlock;
    Node<recordType, keyType> *currentNode;
    std::fstream file;
    char fileName[20];

public:
    BTree()
    {
        currentNode = nullptr;
        std::cout << "Insert file name: ";
        std::cin >> fileName;
        file.open(fileName, std::fstream::in | std::fstream::out | std::fstream::binary);
        if (!file)
        {
            std::cout << "File not found! Insert degree of B-Tree: ";
            std::cin >> N;
            createNewFile(N, fileName);
        }
        else
        {
            file.read((char *)&rootIndex, sizeof(rootIndex));
            file.read((char *)&N, sizeof(N));
            file.read((char *)&sizeBlock, sizeof(sizeBlock));
            file.read((char *)&writeIndex, sizeof(writeIndex));
            setCurrentNodeRoot();
        }
    }

    void createNewFile(int treeN, char* file_name)
    {
        file.open(file_name, std::fstream::binary | std::fstream::trunc | std::fstream::out);
        file.close();
        file.open(file_name, std::fstream::binary | std::fstream::in | std::fstream::out);
        N = treeN;
        rootIndex = 0;
        writeIndex = 1;
        sizeBlock =
                sizeof(bool) +
                2 * sizeof(int) +
                2 * sizeof(indexType) +
                (2 * N - 1) * sizeof(keyType) +
                (2 * N - 1) * sizeof(recordType) +
                2 * N * sizeof(indexType);
        std::size_t remainder = sizeBlock % 16;
        if (remainder)
            sizeBlock += 16 - remainder;
        file.write((char *)&rootIndex, sizeof(rootIndex));
        file.write((char *)&N, sizeof(N));
        file.write((char *)&sizeBlock, sizeof(sizeBlock));
        file.write((char *)&writeIndex, sizeof(writeIndex));
    }

    ~BTree()
    {
        this->file.close();
        if (currentNode)
        {
            delete[] currentNode->keys;
            delete[] currentNode->data;
            delete[] currentNode->children;
            delete currentNode;
        }
    }

    void setCurrentNodeRoot()
    {
        if (currentNode && currentNode->parent)
        {
            delete currentNode;
            currentNode = readNode(rootIndex);
        }
        else
            currentNode = readNode(rootIndex);
    }

    void setNewRootIndex(indexType index)
    {
        rootIndex = index;
        file.seekp(0, this->file.beg);
        file.write((char *)&rootIndex, sizeof(rootIndex));
    }

    void incWriteIndex()
    {
        writeIndex++;
        file.seekp(
                sizeof(rootIndex) +
                sizeof(N) +
                sizeof(sizeBlock),
                this->file.beg);
        file.write((char *)&writeIndex, sizeof(writeIndex));
    }

    void writeNode(indexType index, Node<recordType, keyType>* node)
    {
        file.seekp(index * sizeBlock, this->file.beg);

        file.write((char *)&node->isLeaf, sizeof(node->isLeaf));

        file.write((char *)&node->N, sizeof(node->N));
        file.write((char *)&node->size, sizeof(node->size));

        file.write((char *)&node->parent, sizeof(node->parent));
        file.write((char *)&node->current, sizeof(node->current));

        for (int i = 0; i < (2 * node->N - 1); i++)
            file.write((char *)&node->keys[i], sizeof(node->keys[i]));

        for (int i = 0; i < (2 * node->N); i++)
            file.write((char *)&node->children[i], sizeof(node->children[i]));

        for (int i = 0; i < (2 * node->N - 1); i++)
            file.write((char *)&node->data[i], sizeof(node->data[i]));

        if (index == writeIndex)
            incWriteIndex();
    }

    Node<recordType, keyType>* readNode(indexType index)
    {
        auto node = new Node<recordType, keyType>(N);

        file.seekg(index * sizeBlock, this->file.beg);

        file.read((char *)&node->isLeaf, sizeof(node->isLeaf));

        file.read((char *)&node->N, sizeof(node->N));
        file.read((char *)&node->size, sizeof(node->size));

        file.read((char *)&node->parent, sizeof(node->parent));
        file.read((char *)&node->current, sizeof(node->current));

        for (int i = 0; i < (2 * node->N - 1); i++)
            file.read((char *)&node->keys[i], sizeof(node->keys[i]));

        for (int i = 0; i <(2 * node->N); i++)
            file.read((char *)&node->children[i], sizeof(node->children[i]));

        for (int i = 0; i < (2 * node->N - 1); i++)
            file.read((char *)&node->data[i], sizeof(node->data[i]));

        return node;
    }

    Node<recordType, keyType>* findNodeInChildTree(keyType key, indexType index)
    {
        if (!index)
            return nullptr;
        Node<recordType, keyType>* node = readNode(index);
        for (int i = 0; i < node->size; i++)
            if (key < node->keys[i])
                return findNodeInChildTree(key, node->children[i]);
            else if (key == node->keys[i])
                return node;
        return findNodeInChildTree(key, node->children[node->size]);
    }

    recordType* findRecordInChildTree(keyType key, indexType index)
    {
        if (!index)
            return nullptr;
        delete currentNode;
        currentNode = readNode(index);
        for (int i = 0; i < currentNode->size; i++)
            if (key < currentNode->keys[i])
                return findRecordInChildTree(key, currentNode->children[i]);
            else if (key == currentNode->keys[i])
                return &(currentNode->data[i]);
        return findRecordInChildTree(key, currentNode->children[currentNode->size]);
    }

    recordType* findRecord(keyType key)
    {
        return findRecordInChildTree(key, rootIndex);
    }

    void printRecord(recordType* record)
    {
        if (record)
        {
            std::cout << "Record:\n";
            std::cout << "          F_Name: " << record->firstName << "\n";
            std::cout << "          S_Name: " << record->secondName << "\n";
            std::cout << "          Age   : " << record->age << "\n";
            std::cout << "          Phone : " << record->telephone << "\n";
        }
        else
        {
            std::cout << "Record is empty!\n";
        }
    }

    void printRecordsInNode(indexType index)
    {
        if (!index)
            return;
        Node<recordType, keyType>* node = new Node<recordType, keyType>(N);
        node = readNode(index);
        for (int i = 0; i < node->size; i++)
        {
            printRecordsInNode(node->children[i]);
            printRecord(&(node->data[i]));
        }
        printRecordsInNode(node->children[node->size]);
        delete node;
    }

    void printRecords()
    {
        if (!rootIndex)
        {
            std::cout << "________EMPTY_TREE________\n";
            return;
        }
        else
        {
            std::cout << "___________RECORDS__________\n";
            printRecordsInNode(rootIndex);
            std::cout << "___________________________\n";
        }
    }

    void printNode(indexType index, int count)
    {
        Node<recordType, keyType>* node = new Node<recordType, keyType>(N);
        node = readNode(index);
        std::cout << "{ ";
        for (int i = 0; i < (2 * N - 2); i++)
        {
            printf("%3d", node->keys[i]);
            if (i != (2 * N - 3))
                std::cout << " | ";
        }
        std::cout << " }\n";
        count += 12 * N - 11;
        for (int i = 0; i <= node->size; i++)
        {
            if (node->children[i])
            {
                for (int j = 0; j < count; j++)
                    std::cout << " ";
                printNode(node->children[i], count);
            }
        }
        delete node;
    }

    void printTree()
    {
        if (!rootIndex)
        {
            std::cout << "________EMPTY_TREE________\n";
            return;
        }
        else
        {
            int count = 0;
            std::cout << "___________B-TREE_________\n";
            printNode(rootIndex, count);
            std::cout << "__________________________\n";
        }
    }

    void insertRecord(Node<recordType, keyType>* node, recordType* record, indexType child = 0)
    {
        int i;
        auto *_keys = new keyType[2 * N - 1];
        auto *_children = new indexType[2 * N];
        auto *_data = new recordType[2 * N - 1];
        for (i = 0; i < (2 * N - 1); i++)
            _keys[i] = 0;
        for (i = 0; i < (2 * N); i++)
            _children[i] = 0;
        for (i = 0; i < node->size; i++)
        {
            if (record->telephone > node->keys[i])
            {
                _keys[i] = node->keys[i];
                _children[i] = node->children[i];
                _data[i] = node->data[i];
            }
            else
                break;
        }
        _keys[i] = record->telephone;
        _children[i] = child;
        _data[i] = *record;
        node->size++;
        for (i++; i < node->size; i++)
        {
            _keys[i] = node->keys[i-1];
            _data[i] = node->data[i-1];
            _children[i] = node->children[i-1];
        }
        _children[i] = node->children[i-1];
        delete[] node->keys;
        delete[] node->children;
        delete[] node->data;
        node->keys = _keys;
        node->children = _children;
        node->data = _data;
    }

    Node<recordType, keyType>* insertInChildTree(indexType index, recordType* record)
    {
        if (!index)
            return nullptr;
        Node<recordType, keyType>* node = readNode(index);
        if (!node->isLeaf)
        {
            Node<recordType, keyType>* childNode;
            int i;
            for (i = 0; i < node->size; i++)
            {
                if (record->telephone < node->keys[i])
                {
                    indexType next = node->children[i];
                    childNode = insertInChildTree(next, record);
                    break;
                }
            }
            if (i == node->size)
            {
                indexType next = node->children[node->size];
                childNode = insertInChildTree(next, record);
            }
            if (childNode && childNode->size == (2 * N - 1))
            {
                auto *_keys = new keyType[2 * N - 1];
                auto *_children = new indexType[2 * N];
                auto *_data = new recordType[2 * N - 1];
                for (i = 0; i < (2 * N - 1); i++)
                    _keys[i] = 0;
                for (i = 0; i < (2 * N); i++)
                    _children[i] = 0;
                auto* newNode = new Node<recordType, keyType>(N);
                newNode->isLeaf = childNode->isLeaf;
                newNode->parent = node->current;
                newNode->current = writeIndex;
                newNode->size = N - 1;
                for (i = 0; i < (N - 1); i++)
                {
                    newNode->keys[i] = childNode->keys[i];
                    newNode->children[i] = childNode->children[i];
                    newNode->data[i] = childNode->data[i];
                }
                newNode->children[i] = childNode->children[i];
                recordType* movedDataUp = &childNode->data[i];
                insertRecord(node, movedDataUp, newNode->current);
                if (!newNode->isLeaf)
                {
                    for (int j = 0; j <= newNode->size; j++)
                    {
                        Node<recordType, keyType>* updateChildNode = readNode(newNode->children[i]);
                        updateChildNode->parent = newNode->current;
                        writeNode(updateChildNode->current, updateChildNode);
                        delete updateChildNode;
                    }
                }
                writeNode(newNode->current, newNode);
                delete newNode;
                int j = 0;
                for (i++; j < (N - 1); i++, j++)
                {
                    _keys[j] = childNode->keys[i];
                    _children[j] = childNode->children[i];
                    _data[j] = childNode->data[i];
                }
                _children[j] = childNode->children[i];
                delete[] childNode->keys;
                delete[] childNode->children;
                delete[] childNode->data;
                childNode->keys = _keys;
                childNode->children = _children;
                childNode->data = _data;
                childNode->size = N - 1;
                writeNode(childNode->current, childNode);
                delete childNode;
            }
        }
        else
        {
            insertRecord(node, record);
        }
        writeNode(node->current, node);
        return node;
    }

    void createRootAndInsert(recordType* record, indexType childLeft = 0, indexType childRight = 0)
    {
        bool isLeaf;
        if (!rootIndex)
            isLeaf = true;
        else
            isLeaf = false;
        setNewRootIndex(writeIndex);
        auto* newRoot = new Node<recordType, keyType>(N);
        newRoot->isLeaf = isLeaf;
        newRoot->size = 1;
        newRoot->current = rootIndex;
        newRoot->keys[0] = record->telephone;
        newRoot->data[0] = *record;
        newRoot->children[0] = childLeft;
        newRoot->children[1] = childRight;
        writeNode(newRoot->current, newRoot);
        delete newRoot;
    }

    void insert(recordType* record)
    {
        if (rootIndex == 0)
            createRootAndInsert(record);
        else
        {
            delete currentNode;
            currentNode = insertInChildTree(rootIndex, record);
            if (currentNode->size == (2 * N - 1))
            {
                int i;
                auto *_keys = new keyType[2 * N - 1];
                auto *_children = new indexType[2 * N];
                auto *_data = new recordType[2 * N - 1];
                for (i = 0; i < (2 * N - 1); i++)
                    _keys[i] = 0;
                for (i = 0; i < (2 * N); i++)
                    _children[i] = 0;
                auto* newNode = new Node<recordType, keyType>(N);
                newNode->isLeaf = currentNode->isLeaf;
                newNode->parent = writeIndex + 1;
                newNode->current = writeIndex;
                newNode->size = N - 1;
                for (i = 0; i < (N - 1); i++)
                {
                    newNode->keys[i] = currentNode->keys[i];
                    newNode->children[i] = currentNode->children[i];
                    newNode->data[i] = currentNode->data[i];
                }
                newNode->children[i] = currentNode->children[i];
                if (!newNode->isLeaf)
                {
                    for (int j = 0; j <= newNode->size; j++)
                    {
                        Node<recordType, keyType>* updateChildNode = readNode(newNode->children[j]);
                        updateChildNode->parent = newNode->current;
                        writeNode(updateChildNode->current, updateChildNode);
                        delete updateChildNode;
                    }
                }
                writeNode(newNode->current, newNode);
                recordType* movedDataUp = &currentNode->data[i];
                createRootAndInsert(movedDataUp, newNode->current, currentNode->current);
                delete newNode;
                int j = 0;
                for (i++; j < (N - 1); i++, j++)
                {
                    _keys[j] = currentNode->keys[i];
                    _children[j] = currentNode->children[i];
                    _data[j] = currentNode->data[i];
                }
                _children[j] = currentNode->children[i];
                delete[] currentNode->keys;
                delete[] currentNode->children;
                delete[] currentNode->data;
                currentNode->keys = _keys;
                currentNode->children = _children;
                currentNode->data = _data;
                currentNode->size = N - 1;
                currentNode->parent = rootIndex;
                writeNode(currentNode->current, currentNode);
            }
        }
    }

    recordType* getRecordWithMaxKey(indexType index)
    {
        Node<recordType, keyType>* node = readNode(index);
        while(!node->isLeaf)
            node = readNode(node->children[node->size]);
        recordType* record = &(node->data[node->size - 1]);
        return record;
    }

    recordType* getRecordWithMinKey(indexType index)
    {
        Node<recordType, keyType>* node = readNode(index);
        while(!node->isLeaf)
            node = readNode(node->children[0]);
        recordType* record = &(node->data[0]);
        return record;
    }

    int delInLeaf(indexType index, keyType delKey)
    {
        Node<recordType, keyType>* node = readNode(index);
        int i;
        auto *_keys = new keyType[2 * N - 1];
        auto *_children = new indexType[2 * N];
        auto *_data = new recordType[2 * N - 1];
        for (i = 0; i < (2 * N - 1); i++)
            _keys[i] = 0;
        for (i = 0; i < (2 * N); i++)
            _children[i] = 0;
        for (i = 0; i < node->size; i++)
        {
            if (delKey > node->keys[i])
            {
                _keys[i] = node->keys[i];
                _children[i] = node->children[i];
                _data[i] = node->data[i];
            }
            else
                break;
        }
        node->size--;
        for (; i < node->size; i++)
        {
            _keys[i] = node->keys[i+1];
            _data[i] = node->data[i+1];
            _children[i] = node->children[i+1];
        }
        _children[i] = node->children[i+1];
        delete[] node->keys;
        delete[] node->children;
        delete[] node->data;
        node->keys = _keys;
        node->children = _children;
        node->data = _data;
        writeNode(node->current, node);
        int size = node->size;
        delete node;
        return size;
    }

    void delKeyInChildTree(indexType index, keyType delKey)
    {
        Node<recordType, keyType>* node = findNodeInChildTree(delKey, index);
        if (!node)
            std::cout << "Key not found!!!!\n";
        else if (node->isLeaf)
        {
            if (delInLeaf(node->current, delKey) < N-1)
            {
                fixRules(node->current);
            }
        }
        else
        {
            for (int i = 0; i < node->size; i++)
            {
                if (delKey == node->keys[i])
                {
                    recordType record = *getRecordWithMaxKey(node->children[i]);
                    node->keys[i] = record.telephone;
                    node->data[i] = record;
                    writeNode(node->current, node);
                    delKeyInChildTree(node->children[i], record.telephone);
                    break;
                }
            }
        }
        delete node;
    }

    void fixRules(indexType index)
    {
        Node<recordType, keyType>* node = readNode(index);
        Node<recordType, keyType>* parentNode = readNode(node->parent);
        int i;
        for (i = 0; i < parentNode->size; i++)
        {
            if (node->current == parentNode->children[i])
                break;
        }
        indexType indexParentKey = i;
        Node<recordType, keyType>* brotherLeft = nullptr;
        Node<recordType, keyType>* brotherRight = nullptr;
        if (indexParentKey == 0 || indexParentKey < parentNode->size)
        {
            brotherRight = readNode(parentNode->children[i + 1]);
        }
        else
        {
            brotherLeft = readNode(parentNode->children[i - 1]);
            indexParentKey--;
        }
        if (brotherRight && brotherRight->size >= N)
        {
            recordType recordFromBrother = brotherRight->data[0];
            delInLeaf(brotherRight->current, recordFromBrother.telephone);
            recordType recordParent = parentNode->data[indexParentKey];
            node->size++;
            node->keys[node->size - 1] = recordParent.telephone;
            node->data[node->size - 1] = recordParent;
            parentNode->keys[indexParentKey] = recordFromBrother.telephone;
            parentNode->data[indexParentKey] = recordFromBrother;
            writeNode(node->current, node);
            writeNode(parentNode->current, parentNode);
            delete brotherRight;
        }
        else if (brotherLeft && brotherLeft->size >= N)
        {
            recordType recordFromBrother = brotherLeft->data[brotherLeft->size - 1];
            delInLeaf(brotherLeft->current, recordFromBrother.telephone);
            recordType recordParent = parentNode->data[indexParentKey];
            node->size++;
            for (int j = node->size; j > 0; j--)
            {
                node->keys[j] = node->keys[j - 1];
                node->data[j] = node->data[j - 1];
            }
            node->keys[0] = recordParent.telephone;
            node->data[0] = recordParent;
            parentNode->keys[indexParentKey] = recordFromBrother.telephone;
            parentNode->data[indexParentKey] = recordFromBrother;
            writeNode(node->current, node);
            writeNode(parentNode->current, parentNode);
            delete brotherLeft;
        }
        else
        {
            indexType indexNodeAfterSplit;
            if (brotherRight)
            {
                node->keys[node->size] = parentNode->keys[indexParentKey];
                node->data[node->size] = parentNode->data[indexParentKey];
                i = ++node->size;
                int j;
                for (j = 0; j < brotherRight->size; i++, j++)
                {
                    node->keys[i] = brotherRight->keys[j];
                    node->data[i] = brotherRight->data[j];
                    node->children[i] = brotherRight->children[j];
                }
                node->children[i] = brotherRight->children[j];
                brotherRight->keys = node->keys;
                brotherRight->data = node->data;
                brotherRight->children = node->children;
                brotherRight->size = i;
                if (!brotherRight->isLeaf)
                {
                    for (int j = 0; j <= brotherRight->size; j++)
                    {
                        Node<recordType, keyType>* updateChildNode = readNode(brotherRight->children[j]);
                        updateChildNode->parent = brotherRight->current;
                        writeNode(updateChildNode->current, updateChildNode);
                        delete updateChildNode;
                    }
                }
                writeNode(brotherRight->current, brotherRight);
                indexNodeAfterSplit = brotherRight->current;

                for (i = indexParentKey; i < parentNode->size; i++)
                {
                    parentNode->keys[i] = parentNode->keys[i + 1];
                    parentNode->children[i] = parentNode->children[i + 1];
                    parentNode->data[i] = parentNode->data[i + 1];
                }
                parentNode->children[i] = parentNode->children[i + 1];
                parentNode->size--;
                writeNode(parentNode->current, parentNode);

                delete brotherRight;
            }
            else if (brotherLeft)
            {
                brotherLeft->keys[brotherLeft->size] = parentNode->keys[indexParentKey];
                brotherLeft->data[brotherLeft->size] = parentNode->data[indexParentKey];
                i = ++brotherLeft->size;
                int j;
                for (j = 0; j < node->size; i++, j++)
                {
                    brotherLeft->keys[i] = node->keys[j];
                    brotherLeft->data[i] = node->data[j];
                    brotherLeft->children[i] = node->children[j];
                }
                brotherLeft->children[i] = node->children[j];
                node->keys = brotherLeft->keys;
                node->data = brotherLeft->data;
                node->children = brotherLeft->children;
                node->size = i;
                if (!brotherLeft->isLeaf)
                {
                    for (int j = 0; j <= brotherLeft->size; j++)
                    {
                        Node<recordType, keyType>* updateChildNode = readNode(brotherLeft->children[j]);
                        updateChildNode->parent = brotherLeft->current;
                        writeNode(updateChildNode->current, updateChildNode);
                        delete updateChildNode;
                    }
                }
                writeNode(node->current, node);
                indexNodeAfterSplit = node->current;

                for (i = indexParentKey; i < parentNode->size; i++)
                {
                    parentNode->keys[i] = parentNode->keys[i + 1];
                    parentNode->children[i] = parentNode->children[i + 1];
                    parentNode->data[i] = parentNode->data[i + 1];
                }
                parentNode->children[i] = parentNode->children[i + 1];
                parentNode->size--;
                writeNode(parentNode->current, parentNode);

                delete brotherLeft;
            }
            if (parentNode->size < N - 1)
            {
                if (parentNode->current == rootIndex && parentNode->size == 0)
                {
                    setNewRootIndex(indexNodeAfterSplit);
                }
                else if (parentNode->current != rootIndex)
                {
                    fixRules(parentNode->current);
                }
            }
        }
        delete parentNode;
        delete node;
    }

    void del(keyType delKey)
    {
        delete currentNode;
        currentNode = readNode(rootIndex);
        if (currentNode->isLeaf)
        {
            Node<recordType, keyType>* node = findNodeInChildTree(delKey, rootIndex);
            if (!node)
                std::cout << "Key not found!!!!\n";
            else if (!delInLeaf(currentNode->current, delKey))
            {
                file.close();
                createNewFile(N, fileName);
            }
            delete node;
        }
        else
        {
            delKeyInChildTree(rootIndex, delKey);
        }
    }
};