/*
    20. Составить программу поиска записи с включением в
сильно ветвящемся Б-дереве порядка N. Б-дерево должно
храниться и обрабатываться в файле с прямым доступом (12).
*/

#include "libs/headers/bTree.hpp"

void printMenu()
{
    std::cout << "1 - insert\n";
    std::cout << "10 - insert tree2\n";
    std::cout << "2 - print records\n";
    std::cout << "20 - print records tree2\n";
    std::cout << "3 - find record\n";
    std::cout << "4 - print tree\n";
    std::cout << "40 - print tree2\n";
    std::cout << "5 - del key\n";
    std::cout << "6 - fill tree\n";
    std::cout << "7 - print count keys\n";
    std::cout << "0 - exit\n";
    std::cout << "cmd-> ";
}

int main()
{
    auto* record = new recordType;
    auto* record2 = new recordType2;
    auto tree = BTree<recordType, decltype(record->telephone)>();
    auto tree2 = BTree<recordType2, decltype(record2->telephone)>();
    int countNodes;
    int i;
    bool ok;
    unsigned long long findKey;
    printMenu();
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
                std::cout << "Insert telephone: ";
                std::cin >> record->telephone;
                ok = tree.insert(record);
                if (ok)
                    std::cout << "________INSERT_OK_________\n";
                else
                    std::cout << "______KEY_NOT_UNIQUE______\n";
                break;
            case 10:
                std::cout << "Insert Name: ";
                std::cin >> record2->name;
                std::cout << "Insert birth year: ";
                std::cin >> record2->birthYear;
                std::cout << "Insert telephone: ";
                std::cin >> record2->telephone;
                ok = tree2.insert(record2);
                if (ok)
                    std::cout << "________INSERT_OK_________\n";
                else
                    std::cout << "______KEY_NOT_UNIQUE______\n";
                break;
            case 2:
                tree.printRecords();
                break;
            case 20:
                tree2.printRecords();
                break;
            case 3:
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
            case 40:
                tree2.printTree();
                break;
            case 5:
                unsigned long long delKey;
                std::cout << "\nInsert key for del: ";
                std::cin >> delKey;
                tree.del(delKey);
                break;
            case 6:
                std::cout << "\nInsert count nodes: ";
                std::cin >> countNodes;
                tree.insertRandom(countNodes);
                break;
            case 7:
                std::cout << "___________________________\n";
                std::cout << "Count keys: " << tree.countKeys() << std::endl;
                std::cout << "___________________________\n";
                break;
            default:
                break;
        }
        printMenu();
        std::cin >> i;
    }
    delete record;
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
