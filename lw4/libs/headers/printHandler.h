#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include "fileHandler.h"
#include "node/nodeFS.h"
#include "node/nodeRAM.h"
#include "record/recordPhone.h"
#include "record/recordBirth.h"

constexpr int SIZE_TAB = 4;

template <typename recordType>
class PrintHandler
{
    FileHandler<recordType>& fileHandler;
public:
    explicit PrintHandler(FileHandler<recordType>& refFileHandler) : fileHandler{refFileHandler} {}
    void printRecord(recordType* record);
    void printRecordsInNode(indexType index);
    void printRecords();
    void printNode(Node<recordType>* node);
    void printTab(int count);
    void printNodes(indexType index, int count);
    void printTree();
};

template<typename recordType>
void PrintHandler<recordType>::printTree() {
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

template<typename recordType>
void PrintHandler<recordType>::printNodes(indexType index, int count) {
    Node<recordType>* node = fileHandler.readNode(index);
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

template<typename recordType>
void PrintHandler<recordType>::printTab(int count) {
    if (count)
        for (int j = 0; j < count; j++)
            if (j % SIZE_TAB == 0 && j != 0)
                std::cout << "|";
            else
                std::cout << " ";
}

template<typename recordType>
void PrintHandler<recordType>::printNode(Node<recordType> *node) {
    std::cout << "{ ";
    for (int i = 0; i < (2 * fileHandler.N - 2); i++)
    {
        printf("%11lld", node->keys[i]);
        if (i != (2 * fileHandler.N - 3))
            std::cout << " | ";
    }
    std::cout << " }\n";
}

template<typename recordType>
void PrintHandler<recordType>::printRecords() {
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

template<typename recordType>
void PrintHandler<recordType>::printRecordsInNode(indexType index) {
    if (!index)
        return;
    Node<recordType>* node = fileHandler.readNode(index);
    for (int i = 0; i < node->size; i++)
    {
        printRecordsInNode(node->children[i]);
        printRecord(&(node->data[i]));
    }
    printRecordsInNode(node->children[node->size]);
    delete node;
}

template<typename recordType>
void PrintHandler<recordType>::printRecord(recordType *record) {
    std::cout << "          Record: " << record << std::endl;
}

template<>
void PrintHandler<RecordPhone>::printRecord(RecordPhone *record) {
    if (record)
    {
        std::cout << "Record:" << std::endl;
        std::cout << "          F_Name: " << record->firstName << std::endl;
        std::cout << "          S_Name: " << record->secondName << std::endl;
        std::cout << "          Age   : " << record->age << std::endl;
        std::cout << "          Phone : " << record->telephone << std::endl;
    }
    else
        std::cout << "Record is empty!" << std::endl;
}

template<>
void PrintHandler<RecordBirth>::printRecord(RecordBirth *record) {
    if (record)
    {
        std::cout << "Record:" << std::endl;
        std::cout << "          Name  : " << record->name << std::endl;
        std::cout << "          Birth : " << record->birthYear << std::endl;
        std::cout << "          Phone : " << record->telephone << std::endl;
    }
    else
        std::cout << "Record is empty!" << std::endl;
}
