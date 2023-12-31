#pragma once
#include <string>
#include <cstring>
#include <cstdio>
#include "store.h"
#include "record.h"

constexpr int SIZE_TAB = 4;

template <class recordType, typename linkType>
class PrintHandler
{
private:
    Store<recordType, linkType>& store;
public:
    explicit PrintHandler(Store<recordType, linkType>& refStore) : store(refStore) {}
    void printRecord(recordType& data);
    void printRecordsInNode(linkType& link);
    void printRecords();
    void printNode(Node<recordType, linkType>& node);
    void printTab(int count);
    void printNodes(linkType& link, int count);
    void printTree();
};

template <>
PrintHandler<RecordPhone, linkFS>::PrintHandler(Store<RecordPhone, linkFS>& refStore) : store(refStore) {}

template <>
PrintHandler<RecordBirth, linkFS>::PrintHandler(Store<RecordBirth, linkFS>& refStore) : store(refStore) {}

template <>
PrintHandler<RecordPhone, linkRAM>::PrintHandler(Store<RecordPhone, linkRAM>& refStore) : store(refStore) {}

template <>
PrintHandler<RecordBirth, linkRAM>::PrintHandler(Store<RecordBirth, linkRAM>& refStore) : store(refStore) {}

template <class recordType, typename linkType>
void PrintHandler<recordType, linkType>::printTree() {
    if (!store.readNode)
    {
        std::cout << "________EMPTY_TREE________\n";
        return;
    }
    else
    {
        int count = 0;
        std::cout << "___________B-TREE_________\n";
        printNodes(store.rootIndex, count);
        std::cout << "__________________________\n";
    }
}

template <>
void PrintHandler<RecordPhone, linkFS>::printTree() {
    if (!store.rootIndex)
    {
        std::cout << "________EMPTY_TREE________\n";
        return;
    }
    else
    {
        int count = 0;
        std::cout << "___________B-TREE_________\n";
        printNodes(store.rootIndex, count);
        std::cout << "__________________________\n";
    }
}

template <typename recordType, typename linkType>
void PrintHandler<recordType, linkType>::printNodes(linkType& link, int count) {
    Node<recordType, linkType> node = store.readNode(link);
    printNode(node);
    count += SIZE_TAB;
    for (int i = 0; i <= node.size; i++)
    {
        if (node.children[i])
        {
            printTab(count);
            printNodes(node.children[i], count);
        }
    }
}

template <typename recordType, typename linkType>
void PrintHandler<recordType, linkType>::printTab(int count) {
    if (count)
        for (int j = 0; j < count; j++)
        {
            if (j % SIZE_TAB == 0 && j != 0)
                std::cout << "|";
            else
                std::cout << " ";
        }
}

template <typename recordType, typename linkType>
void PrintHandler<recordType, linkType>::printNode(Node<recordType, linkType>& node) {
    std::cout << "{ ";
    for (int i = 0; i < (2 * store.N - 2); i++)
    {
        printf("%11lld", node.keys[i]);
        if (i != (2 * store.N - 3))
            std::cout << " | ";
    }
    std::cout << " }\n";
}

template <typename recordType, typename linkType>
void PrintHandler<recordType, linkType>::printRecords() {
    if (!store.rootIndex)
    {
        std::cout << "________EMPTY_TREE________\n";
        return;
    }
    else
    {
        std::cout << "___________RECORDS__________\n";
        printRecordsInNode(store.rootIndex);
        std::cout << "___________________________\n";
    }
}

template <typename recordType, typename linkType>
void PrintHandler<recordType, linkType>::printRecordsInNode(linkType& link) {
    if (!link)
        return;
    Node<recordType, linkType> node = store.readNode(link);
    for (int i = 0; i < node.size; i++)
    {
        printRecordsInNode(node.children[i]);
        printRecord(&(node.data[i]));
    }
    printRecordsInNode(node.children[node.size]);
}

template <typename recordType, typename linkType>
void PrintHandler<recordType, linkType>::printRecord(recordType& data) {
    std::cout << "          Record: " << data << std::endl;
}

template<>
void PrintHandler<RecordPhone, linkRAM>::printRecord(RecordPhone& data) {
    std::cout << "Record:" << std::endl;
    std::cout << "          F_Name: " << data.record.firstName << std::endl;
    std::cout << "          S_Name: " << data.record.secondName << std::endl;
    std::cout << "          Age   : " << data.record.age << std::endl;
    std::cout << "          Phone : " << data.record.telephone << std::endl;
}

template<>
void PrintHandler<RecordPhone, linkFS>::printRecord(RecordPhone& data) {
    std::cout << "Record:" << std::endl;
    std::cout << "          F_Name: " << data.record.firstName << std::endl;
    std::cout << "          S_Name: " << data.record.secondName << std::endl;
    std::cout << "          Age   : " << data.record.age << std::endl;
    std::cout << "          Phone : " << data.record.telephone << std::endl;
}

template<>
void PrintHandler<RecordBirth, linkRAM>::printRecord(RecordBirth& data) {
    std::cout << "Record:" << std::endl;
    std::cout << "          Name  : " << data.record.name << std::endl;
    std::cout << "          Birth : " << data.record.birthYear << std::endl;
    std::cout << "          Phone : " << data.record.telephone << std::endl;
}

template<>
void PrintHandler<RecordBirth, linkFS>::printRecord(RecordBirth& data) {
    std::cout << "Record:" << std::endl;
    std::cout << "          Name  : " << data.record.name << std::endl;
    std::cout << "          Birth : " << data.record.birthYear << std::endl;
    std::cout << "          Phone : " << data.record.telephone << std::endl;
}
