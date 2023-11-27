/*
    20. Составить программу поиска записи с включением в
сильно ветвящемся Б-дереве порядка N. Б-дерево должно
храниться и обрабатываться в файле с прямым доступом (12).
*/

#include "libs/headers/bTree.hpp"
#include <cstring>

typedef struct recordStruct
{
    char firstName[32];
    char secondName[32];
    int age;
    unsigned long long telephone;
} recordType;


int main()
{
    char fileName[] = "1.bin";
    auto *record = new recordType;
    record->age = 32;
    record->telephone = 7;
    strcpy(record->firstName, "name");
    strcpy(record->secondName, "surname");
    auto tree = BTree<recordType, decltype(record->telephone)>(3, fileName);
//    tree.insert(record);
//    tree.printRecord(tree.findRecord(10));
//    tree.printRecords();
//    tree.printTree();
    int i;
    std::cout << "1 - insert\n";
    std::cout << "2 - print records\n";
    std::cout << "3 - found record\n";
    std::cout << "4 - print tree\n";
    std::cout << "0 - exit\n";
    std::cout << "cmd-> ";
    std::cin >> i;
    unsigned long long newKey;
    while (i)
    {
        switch (i) {
            case 1:
                std::cout << "insert key: ";
                std::cin >> newKey;
                record->telephone = newKey;
                tree.insert(record);
                tree.printTree();
                break;
            case 2:
                tree.printRecords();
                break;
            case 3:
                std::cout << "insert key for found: ";
                std::cin >> newKey;
                tree.printRecord(tree.findRecord(newKey));
                break;
            case 4:
                tree.printRecords();
                break;
            default:
                break;
        }
        std::cout << "1 - insert\n";
        std::cout << "2 - print records\n";
        std::cout << "3 - found record\n";
        std::cout << "4 - print tree\n";
        std::cout << "0 - exit\n";
        std::cout << "cmd-> ";
        std::cin >> i;
    }
    return 0;
}

//bool isLeaf;
//int N;
//int size;
//indexType parent;
//indexType current;
//keyType *keys;                 [2 * N - 1]
//indexType *children;           [2 * N]
//recordType* data;              [2 * N - 1]
