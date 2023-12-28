#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include "node.h"
#include "fileHandler.h"

constexpr int SIZE_TAB = 4;

template <typename recordType, typename keyType>
class PrintHandler
{
    FileHandler<recordType, keyType>& fileHandler;
public:
    PrintHandler(FileHandler<recordType, keyType>& refFileHandler) : fileHandler{refFileHandler} {}
    void printRecord(recordType* record);
    void printRecordsInNode(indexType index);
    void printRecords();
    int countKeysInNode(indexType index);
    int countKeys();
    void printNode(Node<recordType, keyType>* node);
    void printTab(int count);
    void printNodes(indexType index, int count);
    void printTree();
};

template<typename recordType, typename keyType>
void PrintHandler<recordType, keyType>::printTree() {
    if (!fileHandler.rootIndex)
    {
        std::cout << "________EMPTY_TREE________\n";
        return;
    }
    else
    {
        int count = 0;
        std::cout << "___________B-TREE_________\n";
        printNodes(fileHandler.rootIndex, count);
        std::cout << "__________________________\n";
    }
}

template<typename recordType, typename keyType>
void PrintHandler<recordType, keyType>::printNodes(indexType index, int count) {
    Node<recordType, keyType>* node = fileHandler.readNode(index);
    printNode(node);
    count += SIZE_TAB;
    for (int i = 0; i <= node->size; i++)
    {
        if (node->children[i])
        {
            printTab(count);
            printNodes(node->children[i], count);
        }
    }
    delete node;
}

template<typename recordType, typename keyType>
void PrintHandler<recordType, keyType>::printTab(int count) {
    if (count)
        for (int j = 0; j < count; j++)
            if (j % SIZE_TAB == 0 && j != 0)
                std::cout << "|";
            else
                std::cout << " ";
}

template<typename recordType, typename keyType>
void PrintHandler<recordType, keyType>::printNode(Node<recordType, keyType> *node) {
    std::cout << "{ ";
    for (int i = 0; i < (2 * fileHandler.N - 2); i++)
    {
        printf("%11lld", node->keys[i]);
        if (i != (2 * fileHandler.N - 3))
            std::cout << " | ";
    }
    std::cout << " }\n";
}

template<typename recordType, typename keyType>
int PrintHandler<recordType, keyType>::countKeys() {
    if (!fileHandler.rootIndex)
        return 0;
    else
        return countKeysInNode(fileHandler.rootIndex);
}

template<typename recordType, typename keyType>
int PrintHandler<recordType, keyType>::countKeysInNode(indexType index) {
    int countCurrent = 0;
    if (!index)
        return countCurrent;
    Node<recordType, keyType>* node = fileHandler.readNode(index);
    countCurrent += node->size;
    for (int i = 0; i < node->size; i++)
        countCurrent += countKeysInNode(node->children[i]);
    countCurrent += countKeysInNode(node->children[node->size]);
    delete node;
    return countCurrent;
}

template<typename recordType, typename keyType>
void PrintHandler<recordType, keyType>::printRecords() {
    if (!fileHandler.rootIndex)
    {
        std::cout << "________EMPTY_TREE________\n";
        return;
    }
    else
    {
        std::cout << "___________RECORDS__________\n";
        printRecordsInNode(fileHandler.rootIndex);
        std::cout << "___________________________\n";
    }
}

template<typename recordType, typename keyType>
void PrintHandler<recordType, keyType>::printRecordsInNode(indexType index) {
    if (!index)
        return;
    Node<recordType, keyType>* node = fileHandler.readNode(index);
    for (int i = 0; i < node->size; i++)
    {
        printRecordsInNode(node->children[i]);
        printRecord(&(node->data[i]));
    }
    printRecordsInNode(node->children[node->size]);
    delete node;
}

template<typename recordType, typename keyType>
void PrintHandler<recordType, keyType>::printRecord(recordType *record) {
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
