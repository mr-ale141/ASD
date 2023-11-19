#pragma once
#include <iostream>
#include <fstream>
#include <variant>

typedef unsigned long long index;

template <typename recordType, typename keyType>
class Node
{
public:
    bool isLeaf;
    int N;
    int size;
    index parent;
    index current;
    index next;
    index prev;
    recordType *record;
    keyType *keys;
    index *childs;

    Node(int _N)
    {
        this->isLeaf = false;
        this->N = _N;
        this->size = 0;
        this->parent = 0;
        this->current = 0;
        this->next = 0;
        this->prev = 0;
        this->record = nullptr;

        keyType *_keys = new keyType[N];
        for (int i = 0; i < N; i++)
            _keys[i] = 0;
        this->keys = _keys;

        index *_childs = new index[N];
        for (int i = 0; i < N; i++)
            _childs[i] = 0;
        this->childs = _childs;
    }
};

template <typename recordType, typename keyType>
class BPlusTree
{
    int N;
    index rootIndex;
    index writeIndex;
    std::size_t sizeBlock;
    Node<recordType, keyType> *rootNode;
    std::fstream file;

public:
    BPlusTree(int _N, char *file_name)
    {
        rootNode = nullptr;
        file.open(file_name, std::fstream::in | std::fstream::out | std::fstream::binary);
        if (file)
        {
            file.read((char *)&rootIndex, sizeof(rootIndex));
            file.read((char *)&N, sizeof(N));
            if (N != _N)
            {
                std::cout << "ERROR!!! N(program) != N(file)\n";
                exit(1);
            }
            file.read((char *)&sizeBlock, sizeof(sizeBlock));
            file.read((char *)&writeIndex, sizeof(writeIndex));
            if (rootIndex)
                readBlock(rootIndex, rootNode);
        }
        else
        {
            file.open(file_name, std::fstream::binary | std::fstream::trunc | std::fstream::out);
            file.close();
            file.open(file_name, std::fstream::binary | std::fstream::in | std::fstream::out);
            N = _N;
            rootIndex = 0;
            writeIndex = 1;
            std::size_t sizeRecord =
                sizeof(bool) +
                2 * sizeof(int) +
                4 * sizeof(index) +
                sizeof(recordType);
            std::size_t sizeNode =
                sizeof(bool) +
                2 * sizeof(int) +
                4 * sizeof(index) +
                (N - 1) * sizeof(keyType) +
                N * sizeof(index);
            sizeBlock = (sizeRecord > sizeNode) ? sizeRecord : sizeNode;
            file.write((char *)&rootIndex, sizeof(rootIndex));
            file.write((char *)&N, sizeof(N));
            file.write((char *)&sizeBlock, sizeof(sizeBlock));
            file.write((char *)&writeIndex, sizeof(writeIndex));
        }
    }

    void setNewRootIndex(index newIndex)
    {
        rootIndex = newIndex;
        file.seekp(0, file.beg);
        file.write((char *)&rootIndex, sizeof(rootIndex));
    }

    void incWriteIndex()
    {
        writeIndex++;
        file.seekp(
            sizeof(rootIndex) +
            sizeof(N) +
            sizeof(sizeBlock),
            file.beg);
        file.write((char *)&writeIndex, sizeof(writeIndex));
    }

    void writeBlock(index index, Node<recordType, keyType> *node)
    {
        file.seekp(index * sizeBlock, file.beg);
        if (node->isLeaf)
        {
            file.write((char *)&node->isLeaf, sizeof(node->isLeaf));

            file.write((char *)&node->N, sizeof(node->N));
            file.write((char *)&node->size, sizeof(node->size));

            file.write((char *)&node->parent, sizeof(node->parent));
            file.write((char *)&node->current, sizeof(node->current));
            file.write((char *)&node->next, sizeof(node->next));
            file.write((char *)&node->prev, sizeof(node->prev));

            file.write((char *)node->record, sizeof(*node->record));
        }
        else
        {
            file.write((char *)&node->isLeaf, sizeof(node->isLeaf));

            file.write((char *)&node->N, sizeof(node->N));
            file.write((char *)&node->size, sizeof(node->size));

            file.write((char *)&node->parent, sizeof(node->parent));
            file.write((char *)&node->current, sizeof(node->current));
            file.write((char *)&node->next, sizeof(node->next));
            file.write((char *)&node->prev, sizeof(node->prev));

            for (int i = 0; i < node->N; i++)
                file.write((char *)&node->keys[i], sizeof(node->keys[i]));

            for (int i = 0; i < node->N; i++)
                file.write((char *)&node->childs[i], sizeof(node->childs[i]));
        }
        if (index == writeIndex)
            incWriteIndex();
    }

    void readBlock(std::size_t index, Node<recordType, keyType>* &node)
    {
        file.seekg(index * sizeBlock, file.beg);
        if (!node)
            node = new Node<recordType, keyType>(N);
        file.read((char *)&node->isLeaf, sizeof(node->isLeaf));
        if (node->isLeaf)
        {
            file.read((char *)&node->N, sizeof(node->N));
            file.read((char *)&node->size, sizeof(node->size));

            file.read((char *)&node->parent, sizeof(node->parent));
            file.read((char *)&node->current, sizeof(node->current));
            file.read((char *)&node->next, sizeof(node->next));
            file.read((char *)&node->prev, sizeof(node->prev));
            if (!node->record)
                node->record = new recordType;
            file.read((char *)node->record, sizeof(*(node->record)));
        }
        else
        {
            file.read((char *)&node->N, sizeof(node->N));
            file.read((char *)&node->size, sizeof(node->size));

            file.read((char *)&node->parent, sizeof(node->parent));
            file.read((char *)&node->current, sizeof(node->current));
            file.read((char *)&node->next, sizeof(node->next));
            file.read((char *)&node->prev, sizeof(node->prev));

            for (int i = 0; i < node->N ; i++)
                file.read((char *)&node->keys[i], sizeof(node->keys[i]));

            for (int i = 0; i < node->N; i++)
                file.read((char *)&node->childs[i], sizeof(node->childs[i]));
        }
    }

    void createRoot(recordType *record)
    {
        setNewRootIndex(writeIndex + 1);
        rootNode = new Node<recordType, keyType>(N);
        rootNode->isLeaf = false;
        rootNode->size = 1;
        rootNode->current = rootIndex;
        rootNode->keys[0] = record->telephone;
        rootNode->childs[0] = writeIndex;
        Node<recordType, keyType> *newLeaf = createNewLeaf(record, rootNode);
        writeBlock(newLeaf->current, newLeaf);
        delete newLeaf;
        writeBlock(rootIndex, rootNode);
    }

    Node<recordType, keyType> *createNewLeaf(recordType *record, Node<recordType, keyType> *currNode)
    {
        Node<recordType, keyType> *newLeaf = new Node<recordType, keyType>(N);
        newLeaf->isLeaf = true;
        newLeaf->size = 1;
        newLeaf->parent = currNode->current;
        newLeaf->current = writeIndex;
        newLeaf->record = record;
        newLeaf->keys[0] = record->telephone;
        return newLeaf;
    }

    index getNextIndexBrother(index parentIndex)
    {
        if (!parentIndex)
            return 0;
        index nextIndex = 0;
        Node<recordType, keyType> *buffNode = new Node<recordType, keyType>(N);
        readBlock(parentIndex, buffNode);
        nextIndex = buffNode->childs[0];
        delete buffNode;
        return nextIndex;
    }

    index getPrevIndexBrother(index parentIndex)
    {
        if (!parentIndex)
            return 0;
        index prevIndex = 0;
        Node<recordType, keyType> *buffNode = new Node<recordType, keyType>(N);
        readBlock(parentIndex, buffNode);
        prevIndex = buffNode->childs[buffNode->size - 1];
        delete buffNode;
        return prevIndex;
    }

    void insertNewItem(recordType *record, Node<recordType, keyType> *currNode)
    {
        Node<recordType, keyType> *newLeaf = createNewLeaf(record, currNode);
        Node<recordType, keyType> *newCurrNode = new Node<recordType, keyType>(N);
        readBlock(currNode->current, newCurrNode);
        newCurrNode->size++;

        keyType newKey = newLeaf->keys[0];
        index nextIndex = 0;
        index prevIndex = 0;
        bool find = false;
        for (int i = 0; i < currNode->size; i++)
            if (find)
            {
                newCurrNode->keys[i + 1] = currNode->keys[i];
                newCurrNode->childs[i + 1] = currNode->childs[i];
            }
            else
                if (newKey <= currNode->keys[i])
                {
                    nextIndex = currNode->childs[i];
                    if (!i)
                        prevIndex = getPrevIndexBrother(currNode->parent);
                    newCurrNode->keys[i] = newKey;
                    newCurrNode->childs[i] = newLeaf->current;
                    newCurrNode->keys[i + 1] = currNode->keys[i];
                    newCurrNode->childs[i + 1] = currNode->childs[i];
                    find = true;
                }
                else
                    prevIndex = currNode->childs[i];
        newLeaf->prev = prevIndex;
        newLeaf->next = nextIndex;
        writeBlock(newLeaf->current, newLeaf);
        writeBlock(newCurrNode->current, newCurrNode);
        if (currNode == rootNode)
            readBlock(rootIndex, rootNode);
        delete newCurrNode;
        delete newLeaf;
    }

    void insert(recordType* record)
    {
        if (rootIndex == 0)
            createRoot(record);
        else
        {
            keyType newKey = record->telephone;
            Node<recordType, keyType> *currNode = findNode(newKey);

            if (currNode->size < N)
                insertNewItem(record, currNode);
            else
            {
                std::cout << "I can`t write!!!\n";
            }

        }
    }

    Node<recordType, keyType> *findNode(keyType newKey)
    {
        Node<recordType, keyType> *currNode = rootNode;
        Node<recordType, keyType> *buffNode = new Node<recordType, keyType>(N);
        bool find = false;
        while (!find)
            for (int i = 0; i < currNode->size; i++)
                if (newKey <= currNode->keys[i])
                {
                    readBlock(currNode->childs[i], buffNode);
                    if (buffNode->isLeaf)
                    {
                        find = true;
                        break;
                    }
                    else
                    {
                        currNode = buffNode;
                        break;
                    }
                }
        delete buffNode;
        return currNode;
    }

    // std::size_t del(T &record);

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
