#pragma once
#include <iostream>
#include <fstream>
#include <random>
#include "node.h"

template <typename recordType, typename keyType>
class FileHandler
{
public:
    int N;
    indexType rootIndex;
    indexType writeIndex;
    std::size_t sizeBlock;
    std::fstream file;
    char fileName[20];
    FileHandler();
    void createNewFile(int treeN, char* file_name);
    void setNewRootIndex(indexType index);
    void incWriteIndex();
    void writeNode(indexType index, Node<recordType, keyType>* node);
    Node<recordType, keyType>* readNode(indexType index);
};

template<typename recordType, typename keyType>
Node<recordType, keyType> *FileHandler<recordType, keyType>::readNode(indexType index) {
    Node<recordType, keyType>* node = new Node<recordType, keyType>(N);

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

template<typename recordType, typename keyType>
void FileHandler<recordType, keyType>::writeNode(indexType index, Node<recordType, keyType> *node) {
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

template<typename recordType, typename keyType>
void FileHandler<recordType, keyType>::incWriteIndex() {
    writeIndex++;
    file.seekp(
            sizeof(rootIndex) +
            sizeof(N) +
            sizeof(sizeBlock),
            this->file.beg);
    file.write((char *)&writeIndex, sizeof(writeIndex));
}

template<typename recordType, typename keyType>
void FileHandler<recordType, keyType>::setNewRootIndex(indexType index) {
    rootIndex = index;
    file.seekp(0, this->file.beg);
    file.write((char *)&rootIndex, sizeof(rootIndex));
}

template<typename recordType, typename keyType>
void FileHandler<recordType, keyType>::createNewFile(int treeN, char *file_name) {
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
    file.seekp(0, this->file.beg);
    file.write((char *)&rootIndex, sizeof(rootIndex));
    file.write((char *)&N, sizeof(N));
    file.write((char *)&sizeBlock, sizeof(sizeBlock));
    file.write((char *)&writeIndex, sizeof(writeIndex));
}

template<typename recordType, typename keyType>
FileHandler<recordType, keyType>::FileHandler() {
    std::cout << "Insert file name: ";
    std::cin >> fileName;
    file.open(fileName, std::fstream::in | std::fstream::out | std::fstream::binary);
    if (!file)
    {
        std::cout << "File not found!\n";
        std::cout << "Create file '" << fileName << "'!!\n";
        std::cout << "Insert degree B-Tree:";
        std::cin >> N;
        createNewFile(N, fileName);
    }
    else
    {
        file.seekg(0, this->file.beg);
        file.read((char *)&rootIndex, sizeof(rootIndex));
        file.read((char *)&N, sizeof(N));
        file.read((char *)&sizeBlock, sizeof(sizeBlock));
        file.read((char *)&writeIndex, sizeof(writeIndex));
    }
}
