#pragma once
#include <fstream>
#include <iostream>
#include "node.h"

typedef unsigned long long linkFS;

template<typename recordType>
using linkRAM = std::shared_ptr<NodeRAM<recordType>>;

template<typename recordType, typename linkType = linkRAM<recordType>>
class Store
{
public:
    int N;
    linkType root;
    linkType nextLink;
    linkType afterNextLink;
    Store() : root(nullptr), nextLink(nullptr), afterNextLink(nullptr)
    {
        std::cout << "Insert degree B-Tree:";
        std::cin >> N;
        nextLink = std::make_shared<NodeRAM<recordType>>(N);
        afterNextLink = std::make_shared<NodeRAM<recordType>>(N);
    }
    linkType getNextLink()
    {
        return nextLink;
    }
    linkType getAfterNextLink()
    {
        return afterNextLink;
    }
    void reset()
    {
        root = nullptr;
    }
    NodeRAM<recordType> getNewNode()
    {
        NodeRAM<recordType> newNode(N);
        return newNode;
    }
    linkType getZeroLink()
    {
        return nullptr;
    }
    void writeNode(linkType link, NodeRAM<recordType>& node)
    {
        if (link == nextLink)
        {
            nextLink = afterNextLink;
            afterNextLink = std::make_shared<NodeRAM<recordType>>(N);
        }
        link->isLeaf = node.isLeaf;
        link->N = node.N;
        link->size = node.size;
        link->keys = std::move(node.keys);
        link->data = std::move(node.data);
        link->parent = node.parent;
        link->current = node.current;
        link->children = std::move(node.children);
    }
    linkType readNode(linkType link)
    {
        return link;
    }
    void setNewRootIndex(linkType newLink)
    {
        root = newLink;
    }
};

template<typename recordType>
class Store<recordType, linkFS>
{
public:
    int N;
    linkFS root;
    std::size_t sizeBlock;
    std::fstream file;
    char fileName[20];
    linkFS writeIndex;
    Store()
    : N{0}, root{0}, writeIndex{0}, sizeBlock{0}, fileName{0}
    {
        std::cout << "Insert file name: ";
        std::cin >> fileName;
        file.open(fileName, std::fstream::in | std::fstream::out | std::fstream::binary);
        if (!file)
        {
            std::cout << "File not found!\n";
            std::cout << "Create file '" << fileName << "'!!\n";
            std::cout << "Insert degree B-Tree:";
            std::cin >> N;
            createNewFile();
        }
        else
        {
            file.seekg(0, this->file.beg);
            file.read((char *)&root, sizeof(root));
            file.read((char *)&N, sizeof(N));
            file.read((char *)&sizeBlock, sizeof(sizeBlock));
            file.read((char *)&writeIndex, sizeof(writeIndex));
        }
    }
    linkFS getNextLink()
    {
        return writeIndex;
    }
    linkFS getAfterNextLink()
    {
        return writeIndex + 1;
    }
    void reset()
    {
        file.close();
        createNewFile();
    }
    NodeFS<recordType> getNewNode()
    {
        NodeFS<recordType> newNode(N);
        return newNode;
    }
    linkFS getZeroLink()
    {
        return 0ULL;
    }
    void writeNode(linkFS index, NodeFS<recordType>& node)
    {
        file.seekp(index * sizeBlock, this->file.beg);

        file.write((char *)&node.isLeaf, sizeof(node.isLeaf));

        file.write((char *)&node.N, sizeof(node.N));
        file.write((char *)&node.size, sizeof(node.size));

        file.write((char *)&node.parent, sizeof(node.parent));
        file.write((char *)&node.current, sizeof(node.current));

        for (int i = 0; i < (2 * node.N - 1); i++)
            file.write((char *)&node.keys[i], sizeof(node.keys[i]));

        for (int i = 0; i < (2 * node.N); i++)
            file.write((char *)&node.children[i], sizeof(node.children[i]));

        for (int i = 0; i < (2 * node.N - 1); i++)
            file.write((char *)&node.data[i], sizeof(node.data[i]));

        if (index == writeIndex)
            incWriteIndex();
    }
    std::shared_ptr<NodeFS<recordType>> readNode(linkFS index)
    {
        if (!index) return nullptr;

        auto node = std::make_shared<NodeFS<recordType>>(N);

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
    void createNewFile()
    {
        file.open(fileName, std::fstream::binary | std::fstream::trunc | std::fstream::out);
        file.close();
        file.open(fileName, std::fstream::binary | std::fstream::in | std::fstream::out);
        root = 0;
        writeIndex = 1;
        sizeBlock =
                sizeof(bool) +
                2 * sizeof(int) +
                2 * sizeof(linkFS) +
                (2 * N - 1) * sizeof(keyType) +
                (2 * N - 1) * sizeof(recordType) +
                2 * N * sizeof(linkFS);
        std::size_t remainder = sizeBlock % 16;
        if (remainder)
            sizeBlock += 16 - remainder;
        file.seekp(0, this->file.beg);
        file.write((char *)&root, sizeof(root));
        file.write((char *)&N, sizeof(N));
        file.write((char *)&sizeBlock, sizeof(sizeBlock));
        file.write((char *)&writeIndex, sizeof(writeIndex));
    }
    void setNewRootIndex(linkFS newIndex)
    {
        root = newIndex;
        file.seekp(0, this->file.beg);
        file.write((char *)&root, sizeof(root));
    }
    void incWriteIndex()
    {
        writeIndex++;
        file.seekp(
                sizeof(root) +
                sizeof(N) +
                sizeof(sizeBlock),
                this->file.beg);
        file.write((char *)&writeIndex, sizeof(writeIndex));
    }
};