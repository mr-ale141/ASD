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
        this->next = 0;
        this->prev = 0;
        this->record = nullptr;

        keyType *_keys = new keyType[N - 1];
        for (int i = 0; i < (N - 1); i++)
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
                3 * sizeof(index) +
                sizeof(recordType);
            std::size_t sizeNode =
                sizeof(bool) +
                2 * sizeof(int) +
                3 * sizeof(index) +
                N * sizeof(keyType) +
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
            file.write((char *)&node->next, sizeof(node->next));
            file.write((char *)&node->prev, sizeof(node->prev));

            for (int i = 0; i < (node->N - 1) ; i++)
                file.write((char *)&node->keys[i], sizeof(node->keys[i]));

            for (int i = 0; i < node->N; i++)
                file.write((char *)&node->childs[i], sizeof(node->childs[i]));
        }
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
            file.read((char *)&node->next, sizeof(node->next));
            file.read((char *)&node->prev, sizeof(node->prev));

            for (int i = 0; i < (node->N - 1) ; i++)
                file.read((char *)&node->keys[i], sizeof(node->keys[i]));

            for (int i = 0; i < node->N; i++)
                file.read((char *)&node->childs[i], sizeof(node->childs[i]));
        }
    }

    void insert(recordType* record)
    {
        if (rootIndex == 0)
        {
            setNewRootIndex(writeIndex);
            if (!rootNode)
                rootNode = new Node<recordType, keyType>(N);
            rootNode->isLeaf = true;
            rootNode->size = 1;
            rootNode->record = record;

            writeBlock(rootIndex, rootNode);
            incWriteIndex();
        }
        else if (rootNode->isLeaf)
        {
            Node<recordType, keyType> *newLeaf = new Node<recordType, keyType>(N);
            newLeaf->isLeaf = true;
            newLeaf->size = 1;
            newLeaf->parent = writeIndex + 1;
            newLeaf->record = record;

            rootNode->parent = writeIndex + 1;
            writeBlock(rootIndex, rootNode);

            Node<recordType, keyType> *newRoot = new Node<recordType, keyType>(N);
            newRoot->size = ++(rootNode->size);
            if (rootNode->record->telephone <= record->telephone)
            {
                newRoot->keys[0] = rootNode->record->telephone;
                newRoot->keys[1] = record->telephone;
                newRoot->childs[0] = rootIndex;
                newRoot->childs[1] = writeIndex;
            }
            else
            {
                newRoot->keys[0] = record->telephone;
                newRoot->keys[1] = rootNode->record->telephone;
                newRoot->childs[0] = writeIndex;
                newRoot->childs[1] = rootIndex;
            }
            writeBlock(writeIndex, newLeaf);
            incWriteIndex();
            setNewRootIndex(writeIndex);
            writeBlock(writeIndex, newRoot);
            incWriteIndex();
            delete rootNode;
            delete newLeaf;
            rootNode = newRoot;
        }
        else
            std::cout << "I can`t write!!!\n";
    }

    // std::size_t del(T &record);
    // std::size_t find(T &record);

    ~BPlusTree()
    {
        this->file.close();
        if (rootNode)
        {
            if (rootNode->isLeaf)
                delete rootNode->record;
            delete[] rootNode->keys;
            delete[] rootNode->childs;
            delete rootNode;
        }
    }
};
