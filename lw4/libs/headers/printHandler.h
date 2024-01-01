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

template <class recordType, typename linkType>
void PrintHandler<recordType, linkType>::printTree()
{
    if (!store.root)
    {
        std::cout << "________EMPTY_TREE________\n";
        return;
    }
    else
    {
        int count = 0;
        std::cout << "___________B-TREE_________\n";
        printNodes(store.root, count);
        std::cout << "__________________________\n";
    }
}

template <typename recordType, typename linkType>
void PrintHandler<recordType, linkType>::printNodes(linkType& link, int count)
{
    auto node = store.readNode(link);
    printNode(*node);
    count += SIZE_TAB;
    for (int i = 0; i <= node->size; i++)
    {
        if (node->children[i])
        {
            printTab(count);
            printNodes(node->children[i], count);
        }
    }
}

template <typename recordType, typename linkType>
void PrintHandler<recordType, linkType>::printTab(int count)
{
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
void PrintHandler<recordType, linkType>::printNode(Node<recordType, linkType>& node)
{
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
void PrintHandler<recordType, linkType>::printRecords()
{
    if (!store.root)
    {
        std::cout << "________EMPTY_TREE________\n";
        return;
    }
    else
    {
        std::cout << "___________RECORDS__________\n";
        printRecordsInNode(store.root);
        std::cout << "___________________________\n";
    }
}

template <typename recordType, typename linkType>
void PrintHandler<recordType, linkType>::printRecordsInNode(linkType& link)
{
    if (!link)
        return;
    auto node = store.readNode(link);
    for (int i = 0; i < node->size; i++)
    {
        printRecordsInNode(node->children[i]);
        printRecord(node->data[i]);
    }
    printRecordsInNode(node->children[node->size]);
}

template <typename recordType, typename linkType>
void PrintHandler<recordType, linkType>::printRecord(recordType& dataPtr)
{
    std::cout << "          Record: " << dataPtr << std::endl;
}

template<>
void PrintHandler<RecordPhone, linkRAM>::printRecord(RecordPhone& dataPtr)
{
    std::cout << "Record:" << std::endl;
    std::cout << "          F_Name: " << dataPtr.record.firstName << std::endl;
    std::cout << "          S_Name: " << dataPtr.record.secondName << std::endl;
    std::cout << "          Age   : " << dataPtr.record.age << std::endl;
    std::cout << "          Phone : " << dataPtr.record.telephone << std::endl;
}

template<>
void PrintHandler<RecordPhone, linkFS>::printRecord(RecordPhone& dataPtr)
{
    std::cout << "Record:" << std::endl;
    std::cout << "          F_Name: " << dataPtr.record.firstName << std::endl;
    std::cout << "          S_Name: " << dataPtr.record.secondName << std::endl;
    std::cout << "          Age   : " << dataPtr.record.age << std::endl;
    std::cout << "          Phone : " << dataPtr.record.telephone << std::endl;
}

template<>
void PrintHandler<RecordBirth, linkRAM>::printRecord(RecordBirth& dataPtr)
{
    std::cout << "Record:" << std::endl;
    std::cout << "          Name  : " << dataPtr.record.name << std::endl;
    std::cout << "          Birth : " << dataPtr.record.birthYear << std::endl;
    std::cout << "          Phone : " << dataPtr.record.telephone << std::endl;
}

template<>
void PrintHandler<RecordBirth, linkFS>::printRecord(RecordBirth& dataPtr)
{
    std::cout << "Record:" << std::endl;
    std::cout << "          Name  : " << dataPtr.record.name << std::endl;
    std::cout << "          Birth : " << dataPtr.record.birthYear << std::endl;
    std::cout << "          Phone : " << dataPtr.record.telephone << std::endl;
}