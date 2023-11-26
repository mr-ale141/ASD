#pragma once
#include <iostream>
#include <fstream>
#include <variant>

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

public:
    BTree(int treeN, char* file_name)
    {
        currentNode = nullptr;
        file.open(file_name, std::fstream::in | std::fstream::out | std::fstream::binary);
        if (!file)
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
        else
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
            setCurrentNodeRoot();
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

    void findNode(keyType newKey)
    {
        setCurrentNodeRoot();
        if (currentNode->isLeaf)
            return;
        bool find = false;
        while (!find)
            for (int i = 0; i < currentNode->size; i++)
                if (i == currentNode->size - 1 || newKey <= currentNode->keys[i])
                {
                    indexType next = currentNode->children[i];
                    delete currentNode;
                    currentNode = readNode(next);
                    if (currentNode->isLeaf)
                    {
                        find = true;
                        break;
                    }
                    else
                        break;
                }
    }

    void printRecord(recordType* record)
    {
        std::cout << "Record:\n";
        std::cout << "          F_Name:" << record->firstName << "\n";
        std::cout << "          S_Name:" << record->secondName << "\n";
        std::cout << "          Age   :" << record->age << "\n";
        std::cout << "          Phone :" << record->telephone << "\n";
    }

    void printNode(indexType index)
    {
        if (!index)
            return;
        delete currentNode;
        currentNode = readNode(index);
        for (int i = 0; i < currentNode->size; i++)
        {
            printNode(currentNode->children[i]);
            printRecord(&(currentNode->data[i]));
        }
        printNode(currentNode->size);
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
            std::cout << "___________B-TREE_________\n";
            printNode(rootIndex);
            std::cout << "__________________________\n";
        }
    }

    

    void createRootAndInsert(recordType* record)
    {
        keyType newKey = record->telephone;
        indexType indexRecord = writeRecord(record);
        setNewRootIndex(writeIndex);
        currentNode = new Node<recordType, keyType>(N);
        currentNode->isLeaf = true;
        currentNode->size = 1;
        currentNode->current = rootIndex;
        currentNode->keys[0] = newKey;
        currentNode->children[0] = indexRecord;
        writeNode(rootIndex, currentNode);
    }

    void insertKey(keyType newKey, indexType indexLink, Node<recordType, keyType>* currNode)
    {
        Node<recordType, keyType> *newCurrNode = readNode(currNode->current);
        newCurrNode->size++;
        bool find = false;
        for (int i = 0; i < currNode->size; i++)
            if (find)
            {
                newCurrNode->keys[i + 1] = currNode->keys[i];
                newCurrNode->children[i + 1] = currNode->children[i];
            }
            else
            {
                if (newKey <= currNode->keys[i])
                {
                    newCurrNode->keys[i] = newKey;
                    newCurrNode->children[i] = indexLink;
                    newCurrNode->keys[i + 1] = currNode->keys[i];
                    newCurrNode->children[i + 1] = currNode->children[i];
                    find = true;
                }
                else if (i == (currNode->size - 1))
                {
                    newCurrNode->keys[i + 1] = newKey;
                    newCurrNode->children[i + 1] = indexLink;
                    find = true;
                }
            }
        writeNode(newCurrNode->current, newCurrNode);
        delete newCurrNode;
    }

    void splitLeafAndInsertRecord(recordType *record, Node<recordType, keyType>* currNode)
    {
        indexType indexRecord = writeRecord(record);
        keyType newKey = record->telephone;

        Node<recordType, keyType>* newLeaf = new Node<recordType, keyType>(N);
        newLeaf->isLeaf = true;
        newLeaf->parent = currNode->parent;
        newLeaf->current = writeIndex;
        newLeaf->children[N - 1] = currNode->current;

        Node<recordType, keyType>* newCurrNode = new Node<recordType, keyType>(N);
        newCurrNode->isLeaf = true;
        newCurrNode->parent = currNode->parent;
        newCurrNode->current = currNode->current;
        newCurrNode->children[N - 1] = currNode->children[N - 1];

        int i;
        for (i = 0; i < (currNode->size - 1); i++)
        {
            if (newKey <= currNode->keys[i])
            {
                newLeaf->keys[i] = newKey;
                newLeaf->children[i] = indexRecord;
                newLeaf->size = i + 1;
                break;
            }
            else
            {
                newLeaf->keys[i] = currNode->keys[i];
                newLeaf->children[i] = currNode->children[i];
                newLeaf->size = i + 1;
            }
        }

        if (i == (currNode->size - 1))
        {
            newCurrNode->keys[0] = newKey;
            newCurrNode->children[0] = indexRecord;
            newCurrNode->size = 1;
        }
        else
        {
            for (int j = 0; i < (currNode->size - 1); i++, j++)
            {
                newCurrNode->keys[j] = currNode->keys[i];
                newCurrNode->children[j] = currNode->children[i];
                newCurrNode->size = j + 1;
            }
        }

        writeNode(newCurrNode->current, newCurrNode);
        writeNode(newLeaf->current, newLeaf);

        keyType keySplitter = newLeaf->keys[newLeaf->size - 1];

        if (currNode->parent == 0)
        {
            setNewRootIndex(writeIndex + 1);
            currentNode = new Node<recordType, keyType>(N);
            currentNode->size = 1;
            currentNode->current = rootIndex;
            currentNode->keys[0] = keySplitter;
            currentNode->children[0] = newLeaf->current;
            currentNode->children[N - 1] = newCurrNode->current;

            newLeaf->parent = rootIndex;
            newCurrNode->parent = rootIndex;
            writeNode(rootIndex, currentNode);
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
        Node<recordType, keyType>* parentNode = readNode(parentIndex);
        if (parentNode->size < (N - 1))
            insertKey(keySplitter, newChildIndex, parentNode);
        else
        {
            Node<recordType, keyType>* newNode = new Node<recordType, keyType>(N);
            newNode->parent = parentNode->parent;
            newNode->current = writeIndex;

            Node<recordType, keyType>* newParentNode = new Node<recordType, keyType>(N);
            newParentNode->parent = parentNode->parent;
            newParentNode->current = parentNode->current;

            int i;
            for (i = 0; i < (parentNode->size - 1); i++)
            {
                if (keySplitter <= parentNode->keys[i])
                {
                    newNode->keys[i] = keySplitter;
                    newNode->children[i] = newChildIndex;
                    newNode->size = i + 1;
                    break;
                }
                else
                {
                    newNode->keys[i] = parentNode->keys[i];
                    newNode->children[i] = parentNode->children[i];
                    newNode->size = i + 1;
                }
            }

            if (i == (parentNode->size - 1))
            {
                newParentNode->keys[0] = keySplitter;
                newParentNode->children[0] = newChildIndex;
                newParentNode->size = 1;
            }
            else
            {
                for (int j = 0; i < (parentNode->size - 1); i++, j++)
                {
                    newParentNode->keys[j] = parentNode->keys[i];
                    newParentNode->children[j] = parentNode->children[i];
                    newParentNode->size = j + 1;
                }
            }

            writeNode(newParentNode->current, newParentNode);
            writeNode(newNode->current, newNode);

            keySplitter = newNode->keys[newNode->size - 1];
            if (parentNode->parent == 0)
            {
                setNewRootIndex(writeIndex + 1);
                currentNode = new Node<recordType, keyType>(N);
                currentNode->size = 1;
                currentNode->current = rootIndex;
                currentNode->keys[0] = keySplitter;
                currentNode->children[0] = newNode->current;
                currentNode->children[N - 1] = newParentNode->current;

                newNode->parent = rootIndex;
                newParentNode->parent = rootIndex;
                writeNode(rootIndex, currentNode);
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
            Node<recordType, keyType> *currNode = findNode(newKey);
            if (currNode->size < (N - 1))
                insertRecord(record, currNode);
            else
                splitLeafAndInsertRecord(record, currNode);
        }
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
};