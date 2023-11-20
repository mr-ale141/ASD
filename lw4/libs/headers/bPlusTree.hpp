#pragma once
#include <iostream>
#include <fstream>
#include <variant>

typedef unsigned long long indexType;

template <typename keyType>
class Node
{
public:
    bool isLeaf;
    int N;
    int size;
    indexType parent;
    indexType current;
    keyType *keys;
    indexType *childs;

    explicit Node(int treeN)
    {
        isLeaf = false;
        N = treeN;
        size = 0;
        parent = 0;
        current = 0;

        auto *_keys = new keyType[N - 1];
        for (int i = 0; i < (N - 1); i++)
            _keys[i] = 0;
        keys = _keys;

        auto *_childs = new indexType[N];
        for (int i = 0; i < N; i++)
            _childs[i] = 0;
        childs = _childs;
    }
};

template <typename recordType, typename keyType>
class BPlusTree
{
    int N;
    indexType rootIndex;
    indexType writeIndex;
    std::size_t sizeBlock;
    Node<keyType> *rootNode;
    std::fstream file;

public:
    BPlusTree(int treeN, char* file_name)
    {
        rootNode = nullptr;
        file.open(file_name, std::fstream::in | std::fstream::out | std::fstream::binary);
        if (file)
        {
            file.read((char *)&rootIndex, sizeof(rootIndex));
            file.read((char *)&N, sizeof(N));
            if (N != treeN)
            {
                std::cout << "ERROR!!! N(program) != N(file)\n";
                exit(1);
            }
            file.read((char *)&sizeBlock, sizeof(sizeBlock));
            file.read((char *)&writeIndex, sizeof(writeIndex));
            if (rootIndex)
                rootNode = readNode(rootIndex);
        }
        else
        {
            file.open(file_name, std::fstream::binary | std::fstream::trunc | std::fstream::out);
            file.close();
            file.open(file_name, std::fstream::binary | std::fstream::in | std::fstream::out);
            N = treeN;
            rootIndex = 0;
            writeIndex = 1;
            std::size_t sizeRecord = sizeof(recordType);
            std::size_t sizeNode =
                sizeof(bool) +
                2 * sizeof(int) +
                2 * sizeof(indexType) +
                (N - 1) * sizeof(keyType) +
                N * sizeof(indexType);
            sizeBlock = (sizeRecord > sizeNode) ? sizeRecord : sizeNode;
            std::size_t remainder = sizeBlock % 16;
            if (remainder)
                sizeBlock += 16 - remainder;
            file.write((char *)&rootIndex, sizeof(rootIndex));
            file.write((char *)&N, sizeof(N));
            file.write((char *)&sizeBlock, sizeof(sizeBlock));
            file.write((char *)&writeIndex, sizeof(writeIndex));
        }
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

    indexType writeRecord(recordType* record)
    {
        indexType index = writeIndex;
        file.seekp(index * sizeBlock, this->file.beg);
        file.write((char *)record, sizeof(*record));
        incWriteIndex();
        return index;
    }

    void writeNode(indexType index, Node<keyType>* node)
    {
        file.seekp(index * sizeBlock, this->file.beg);

        file.write((char *)&node->isLeaf, sizeof(node->isLeaf));

        file.write((char *)&node->N, sizeof(node->N));
        file.write((char *)&node->size, sizeof(node->size));

        file.write((char *)&node->parent, sizeof(node->parent));
        file.write((char *)&node->current, sizeof(node->current));

        for (int i = 0; i < (node->N - 1); i++)
            file.write((char *)&node->keys[i], sizeof(node->keys[i]));

        for (int i = 0; i < node->N; i++)
            file.write((char *)&node->childs[i], sizeof(node->childs[i]));
        if (index == writeIndex)
            incWriteIndex();
    }

    recordType* readRecord(indexType index)
    {
        auto record = new recordType;
        file.seekg(index * sizeBlock, this->file.beg);
        file.read((char *)record, sizeof(*record));
        return record;
    }

    Node<keyType>* readNode(indexType index)
    {
        auto node = new Node<keyType>(N);
        file.seekg(index * sizeBlock, this->file.beg);

        file.read((char *)&node->isLeaf, sizeof(node->isLeaf));

        file.read((char *)&node->N, sizeof(node->N));
        file.read((char *)&node->size, sizeof(node->size));

        file.read((char *)&node->parent, sizeof(node->parent));
        file.read((char *)&node->current, sizeof(node->current));

        for (int i = 0; i < (node->N - 1) ; i++)
            file.read((char *)&node->keys[i], sizeof(node->keys[i]));

        for (int i = 0; i < node->N; i++)
            file.read((char *)&node->childs[i], sizeof(node->childs[i]));
        return node;
    }

    void createRootAndInsert(recordType* record)
    {
        keyType newKey = record->telephone;
        indexType indexRecord = writeRecord(record);
        setNewRootIndex(writeIndex);
        rootNode = new Node<keyType>(N);
        rootNode->isLeaf = true;
        rootNode->size = 1;
        rootNode->current = rootIndex;
        rootNode->keys[0] = newKey;
        rootNode->childs[0] = indexRecord;
        writeNode(rootIndex, rootNode);
    }

    void insertKey(keyType newKey, indexType indexLink, Node<keyType>* currNode)
    {
        Node<keyType> *newCurrNode = readNode(currNode->current);
        newCurrNode->size++;
        bool find = false;
        for (int i = 0; i < currNode->size; i++)
            if (find)
            {
                newCurrNode->keys[i + 1] = currNode->keys[i];
                newCurrNode->childs[i + 1] = currNode->childs[i];
            }
            else
            {
                if (newKey <= currNode->keys[i])
                {
                    newCurrNode->keys[i] = newKey;
                    newCurrNode->childs[i] = indexLink;
                    newCurrNode->keys[i + 1] = currNode->keys[i];
                    newCurrNode->childs[i + 1] = currNode->childs[i];
                    find = true;
                }
                else if (i == (currNode->size - 1))
                {
                    newCurrNode->keys[i + 1] = newKey;
                    newCurrNode->childs[i + 1] = indexLink;
                    find = true;
                }
            }
        writeNode(newCurrNode->current, newCurrNode);
        delete newCurrNode;
    }

    void insertRecord(recordType* record, Node<keyType>* currNode)
    {
        indexType indexRecord = writeRecord(record);
        keyType newKey = record->telephone;
        insertKey(newKey, indexRecord, currNode);
        if (currNode == rootNode)
        {
            delete rootNode;
            rootNode = readNode(rootIndex);
        }
        else
            delete currNode;
    }

    void splitLeafAndInsertRecord(recordType *record, Node<keyType>* currNode)
    {
        indexType indexRecord = writeRecord(record);
        keyType newKey = record->telephone;

        Node<keyType>* newLeaf = new Node<keyType>(N);
        newLeaf->isLeaf = true;
        newLeaf->parent = currNode->parent;
        newLeaf->current = writeIndex;
        newLeaf->childs[N - 1] = currNode->current;

        Node<keyType>* newCurrNode = new Node<keyType>(N);
        newCurrNode->isLeaf = true;
        newCurrNode->parent = currNode->parent;
        newCurrNode->current = currNode->current;
        newCurrNode->childs[N - 1] = currNode->childs[N - 1];

        int i;
        for (i = 0; i < (currNode->size - 1); i++)
        {
            if (newKey <= currNode->keys[i])
            {
                newLeaf->keys[i] = newKey;
                newLeaf->childs[i] = indexRecord;
                newLeaf->size = i + 1;
                break;
            }
            else
            {
                newLeaf->keys[i] = currNode->keys[i];
                newLeaf->childs[i] = currNode->childs[i];
                newLeaf->size = i + 1;
            }
        }

        if (i == (currNode->size - 1))
        {
            newCurrNode->keys[0] = newKey;
            newCurrNode->childs[0] = indexRecord;
            newCurrNode->size = 1;
        }
        else
        {
            for (int j = 0; i < (currNode->size - 1); i++, j++)
            {
                newCurrNode->keys[j] = currNode->keys[i];
                newCurrNode->childs[j] = currNode->childs[i];
                newCurrNode->size = j + 1;
            }
        }

        writeNode(newCurrNode->current, newCurrNode);
        writeNode(newLeaf->current, newLeaf);

        keyType keySplitter = newLeaf->keys[newLeaf->size - 1];

        if (currNode->parent == 0)
        {
            setNewRootIndex(writeIndex + 1);
            rootNode = new Node<keyType>(N);
            rootNode->size = 1;
            rootNode->current = rootIndex;
            rootNode->keys[0] = keySplitter;
            rootNode->childs[0] = newLeaf->current;
            rootNode->childs[N - 1] = newCurrNode->current;

            newLeaf->parent = rootIndex;
            newCurrNode->parent = rootIndex;
            writeNode(rootIndex, rootNode);
        }
        else
            updateParent(
                    currNode->parent,
                    newLeaf->current,
                    keySplitter);

        delete newCurrNode;
        delete currNode;
        delete newLeaf;
    }

    void updateParent(indexType parentIndex, indexType newChildIndex, indexType keySplitter)
    {
        Node<keyType>* parentNode = readNode(parentIndex);
        if (parentNode->size < (N - 1))
            insertKey(keySplitter, newChildIndex, parentNode);
        else
        {
            Node<keyType>* newNode = new Node<keyType>(N);
            newNode->parent = parentNode->parent;
            newNode->current = writeIndex;

            Node<keyType>* newParentNode = new Node<keyType>(N);
            newParentNode->parent = parentNode->parent;
            newParentNode->current = parentNode->current;

            int i;
            for (i = 0; i < (parentNode->size - 1); i++)
            {
                if (keySplitter <= parentNode->keys[i])
                {
                    newNode->keys[i] = keySplitter;
                    newNode->childs[i] = newChildIndex;
                    newNode->size = i + 1;
                    break;
                }
                else
                {
                    newNode->keys[i] = parentNode->keys[i];
                    newNode->childs[i] = parentNode->childs[i];
                    newNode->size = i + 1;
                }
            }

            if (i == (parentNode->size - 1))
            {
                newParentNode->keys[0] = keySplitter;
                newParentNode->childs[0] = newChildIndex;
                newParentNode->size = 1;
            }
            else
            {
                for (int j = 0; i < (parentNode->size - 1); i++, j++)
                {
                    newParentNode->keys[j] = parentNode->keys[i];
                    newParentNode->childs[j] = parentNode->childs[i];
                    newParentNode->size = j + 1;
                }
            }

            writeNode(newParentNode->current, newParentNode);
            writeNode(newNode->current, newNode);

            keySplitter = newNode->keys[newNode->size - 1];
            if (parentNode->parent == 0)
            {
                setNewRootIndex(writeIndex + 1);
                rootNode = new Node<keyType>(N);
                rootNode->size = 1;
                rootNode->current = rootIndex;
                rootNode->keys[0] = keySplitter;
                rootNode->childs[0] = newNode->current;
                rootNode->childs[N - 1] = newParentNode->current;

                newNode->parent = rootIndex;
                newParentNode->parent = rootIndex;
                writeNode(rootIndex, rootNode);
            }
            else
                updateParent(parentNode->parent, newNode->current, keySplitter);

            delete newParentNode;
            delete parentNode;
            delete newNode;
        }
    }

    void insert(recordType* record)
    {
        if (rootIndex == 0)
            createRootAndInsert(record);
        else
        {
            keyType newKey = record->telephone;
            Node<keyType> *currNode = findNode(newKey);
            if (currNode->size < (N - 1))
                insertRecord(record, currNode);
            else
                splitLeafAndInsertRecord(record, currNode);
        }
    }

    Node<keyType>* findNode(keyType newKey)
    {
        if (rootNode->isLeaf)
            return rootNode;
        Node<keyType> *currNode = rootNode;
        bool find = false;
        while (!find)
            for (int i = 0; i < currNode->size; i++)
                if (i == currNode->size - 1 || newKey <= currNode->keys[i])
                {
                    indexType next = currNode->childs[i];
                    if (currNode != rootNode)
                        delete currNode;
                    currNode = readNode(next);
                    if (currNode->isLeaf)
                    {
                        find = true;
                        break;
                    }
                    else
                        break;
                }
        return currNode;
    }

    void printLeafs(Node<keyType>* node)
    {
        int count = 1;
        do {
            std::cout << "IsLeaf : " << "#" << count << "\n";
            std::cout << "N      : " << node->N << "\n";
            std::cout << "Size   : " << node->size << "\n";
            std::cout << "Parent : " << node->parent << "\n";
            std::cout << "Current: " << node->current << "\n";
            for (int i = 0; (i < node->N - 1); i++)
            {
                if (node->childs[i])
                {
                    recordType* record = readRecord(node->childs[i]);
                    std::cout << "Record #" << i + 1 << ":\n";
                    std::cout << "          F_Name:" << record->firstName << "\n";
                    std::cout << "          S_Name:" << record->secondName << "\n";
                    std::cout << "          Age   :" << record->age << "\n";
                    std::cout << "          Phone :" << record->telephone << "\n";
                    delete record;
                }
                else
                    std::cout << "Record #" << i + 1 << ": EMPTY\n";
            }
            std::cout << "___________________\n";
            if (indexType next = node->childs[N - 1])
            {
                if (node != rootNode)
                    delete node;
                node = readNode(next);
            }
            else
                node = nullptr;
            count++;
        } while (node || node == rootNode);
    }

    void printTree()
    {
        if (!rootIndex)
        {
            std::cout << "_______EMPTY_TREE________\n";
            return;
        }
        if (rootNode->isLeaf)
        {
            std::cout << "_______TREE________\n";
            std::cout << "_____ROOT_NODE_____\n";
            printLeafs(rootNode);
        }
        else
        {
            Node<keyType> *node = readNode(rootNode->childs[0]);
            while (!node->isLeaf)
            {
                indexType next = node->childs[0];
                delete node;
                node = readNode(next);
            }
            std::cout << "_______TREE________\n";
            std::cout << "_____ROOT_NODE_____\n";
            printLeafs(node);
        }
    }

    // std::size_t delete(T &record);

    ~BPlusTree()
    {
        this->file.close();
        if (rootNode)
        {
            delete[] rootNode->keys;
            delete[] rootNode->childs;
            delete rootNode;
        }
    }
};
