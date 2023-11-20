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
    BPlusTree(int treeN, char *file_name)
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

    indexType writeRecord(recordType *record)
    {
        indexType index = writeIndex;
        file.seekp(index * sizeBlock, this->file.beg);
        file.write((char *)record, sizeof(*record));
        incWriteIndex();
        return index;
    }

    void writeNode(indexType index, Node<keyType> *node)
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

    void createRoot(recordType *record)
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

    indexType getNextIndexBrother(indexType parentIndex, indexType childIndex)
    {
        return 0;
    }

    indexType getPrevIndexBrother(indexType parentIndex, indexType childIndex)
    {
        if (!parentIndex)
            return 0;
        indexType prevIndex;
        Node<keyType> *buffNode = readNode(parentIndex);
        while (buffNode->childs[0] == childIndex)
        {
            if (buffNode == rootNode)
                break;
            childIndex = buffNode->current;
            indexType parent = buffNode->parent;
            delete buffNode;
            buffNode = readNode(parent);
        }
        if (buffNode == rootNode && buffNode->childs[0] == childIndex)
            return  0;
        for (int i = 0; i < buffNode->size; i++)
            if (buffNode->childs[i] == childIndex)
                prevIndex = buffNode->childs[i - 1];
        if (buffNode != rootNode)
            delete buffNode;
        buffNode = readNode(prevIndex);
        while (!buffNode->isLeaf)
        {
            prevIndex = buffNode->childs[buffNode->size - 1];
            if (buffNode != rootNode)
                delete buffNode;
            buffNode = readNode(prevIndex);
        }
        prevIndex = buffNode->childs[buffNode->size - 1];
        if (buffNode != rootNode)
            delete buffNode;
        return prevIndex;
    }

    void insertItem(recordType *record, Node<keyType> *currNode)
    {
        indexType indexRecord = writeRecord(record);
        Node<keyType> *newCurrNode = readNode(currNode->current);
        newCurrNode->size++;
        keyType newKey = record->telephone;
        bool find = false;
        int i;
        for (i = 0; i < currNode->size; i++)
            if (find)
            {
                newCurrNode->keys[i + 1] = currNode->keys[i];
                newCurrNode->childs[i + 1] = currNode->childs[i];
            }
            else
                if (newKey <= currNode->keys[i])
                {
                    newCurrNode->keys[i] = newKey;
                    newCurrNode->childs[i] = indexRecord;
                    newCurrNode->keys[i + 1] = currNode->keys[i];
                    newCurrNode->childs[i + 1] = currNode->childs[i];
                    find = true;
                }
                else if (i == (currNode->size - 1))
                {
                    newCurrNode->keys[i + 1] = newKey;
                    newCurrNode->childs[i + 1] = indexRecord;
                    find = true;
                }
        writeNode(newCurrNode->current, newCurrNode);
        if (currNode == rootNode)
        {
            delete rootNode;
            rootNode = readNode(rootIndex);
        }
        else
            delete currNode;
        delete newCurrNode;
    }

    void insert(recordType* record)
    {
        if (rootIndex == 0)
            createRoot(record);
        else
        {
            keyType newKey = record->telephone;
            Node<keyType> *currNode = findNode(newKey);
            if (currNode->size < (N - 1))
                insertItem(record, currNode);
            else
            {
                std::cout << "I can`t write!!!\n";
            }
        }
    }

    Node<keyType> *findNode(keyType newKey)
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

    void printLeafs(Node<keyType> *node)
    {

        do {
            std::cout << "IsLeaf : " << (rootNode->isLeaf ? "true" : "false") << "\n";
            std::cout << "N      : " << rootNode->N << "\n";
            std::cout << "Size   : " << rootNode->size << "\n";
            std::cout << "Parent : " << rootNode->parent << "\n";
            std::cout << "Current: " << rootNode->current << "\n";
            for (int i = 0; (i < rootNode->N - 1); i++)
            {
                if (rootNode->childs[i])
                {
                    recordType* record = readRecord(rootNode->childs[i]);
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
