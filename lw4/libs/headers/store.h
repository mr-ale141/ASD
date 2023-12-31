#pragma once
#include <fstream>
#include <iostream>
#include "node.h"

template<typename recordType, typename linkType = linkRAM>
class Store {
public:
    Store() : root(nullptr) {}
    void writeNode(linkType& link, Node<recordType, linkType>& node)
    {
        link = std::make_shared<Node<recordType, linkType>>(node.N);
        *link = node;
    }
    std::shared_ptr<Node<recordType, linkType>> readNode(linkType& link)
    {
        return link;
    }
private:
    std::shared_ptr<Node<recordType, linkType>> root;
};

template<typename recordType>
class Store<recordType, linkFS> {
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
    void writeNode(linkFS& index, Node<recordType, linkFS>& node)
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
            file.write((char *)&node.data[i].record, sizeof(node.data[i].record));

        if (index == writeIndex)
            incWriteIndex();
    }
    std::shared_ptr<Node<recordType, linkFS>> readNode(linkFS& index)
    {
        auto node = std::make_shared<Node<recordType, linkFS>>(N);

        file.seekg(index * sizeBlock, this->file.beg);

        file.read((char *)&node.isLeaf, sizeof(node.isLeaf));

        file.read((char *)&node.N, sizeof(node.N));
        file.read((char *)&node.size, sizeof(node.size));

        file.read((char *)&node.parent, sizeof(node.parent));
        file.read((char *)&node.current, sizeof(node.current));

        for (int i = 0; i < (2 * node.N - 1); i++)
            file.read((char *)&node.keys[i], sizeof(node.keys[i]));

        for (int i = 0; i <(2 * node.N); i++)
            file.read((char *)&node.children[i], sizeof(node.children[i]));

        for (int i = 0; i < (2 * node.N - 1); i++)
            file.read((char *)&node.data[i].record, sizeof(node.data[i].record));

        return node;
    }
    void createNewFile() {
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
    void setNewRootIndex(linkFS& newIndex) {
        root = newIndex;
        file.seekp(0, this->file.beg);
        file.write((char *)&root, sizeof(root));
    }
    void incWriteIndex() {
        writeIndex++;
        file.seekp(
                sizeof(root) +
                sizeof(N) +
                sizeof(sizeBlock),
                this->file.beg);
        file.write((char *)&writeIndex, sizeof(writeIndex));
    }
};