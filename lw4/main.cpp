/*
    20. Составить программу поиска записи с включением в
сильно ветвящемся Б-дереве порядка N. Б-дерево должно
храниться и обрабатываться в файле с прямым доступом (12).
*/

#include "libs/headers/bTree.hpp"

typedef struct recordStruct
{
    char firstName[32];
    char secondName[32];
    int age;
    unsigned long long telephone;
} recordType;


int main()
{
    auto *record = new recordType;
    auto tree = BTree<recordType, decltype(record->telephone)>();
    int countNodes;
    int i;
    std::cout << "1 - insert\n";
    std::cout << "2 - print records\n";
    std::cout << "3 - find record\n";
    std::cout << "4 - print tree\n";
    std::cout << "5 - del key\n";
    std::cout << "6 - fill tree\n";
    std::cout << "0 - exit\n";
    std::cout << "cmd-> ";
    std::cin >> i;
    while (i)
    {
        switch (i) {
            case 1:
                std::cout << "Insert First Name: ";
                std::cin >> record->firstName;
                std::cout << "Insert Second Name: ";
                std::cin >> record->secondName;
                std::cout << "Insert age: ";
                std::cin >> record->age;
                std::cout << "Insert key: ";
                std::cin >> record->telephone;
                tree.insert(record);
                std::cout << "________INSERT_OK_________\n";
                break;
            case 2:
                tree.printRecords();
                break;
            case 3:
                unsigned long long findKey;
                std::cout << "\nInsert key for found: ";
                std::cin >> findKey;
                record = tree.findRecord(findKey, true);
                std::cout << "___________RECORD__________\n";
                tree.printRecord(record);
                std::cout << "___________________________\n";
                break;
            case 4:
                tree.printTree();
                break;
            case 5:
                unsigned long long delKey;
                std::cout << "\nInsert key for del: ";
                std::cin >> delKey;
                tree.del(delKey);
                tree.printTree();
                break;
            case 6:
                std::cout << "\nInsert count nodes: ";
                std::cin >> countNodes;
                tree.insertRandom(countNodes);
                break;
            default:
                break;
        }
        std::cout << "1 - insert\n";
        std::cout << "2 - print records\n";
        std::cout << "3 - find record\n";
        std::cout << "4 - print tree\n";
        std::cout << "5 - del key\n";
        std::cout << "6 - fill tree\n";
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
