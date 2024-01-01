/*
    20. Составить программу поиска записи с включением в
сильно ветвящемся Б-дереве порядка N. Б-дерево должно
храниться и обрабатываться в файле с прямым доступом (12).
*/

#include <random>
#include "libs/headers/bTree.hpp"

struct PRNG
{
    std::mt19937 engine;
};
PRNG generator;

unsigned getRandomInt(unsigned int minValue, unsigned int maxValue) {
    if (minValue < maxValue)
    {
        std::uniform_int_distribution<unsigned> distribution(minValue, maxValue);
        return distribution(generator.engine);
    }
    else
    {
        std::cout << "Error random INT: minValue > maxValue !!!\n";
        exit(1);
    }
}

unsigned long long getRandomULL(unsigned long long int minValue, unsigned long long int maxValue) {
    if (minValue < maxValue)
    {
        std::uniform_int_distribution<unsigned long long> distribution(minValue, maxValue);
        return distribution(generator.engine);
    }
    else
    {
        std::cout << "Error random INT: minValue > maxValue !!!\n";
        exit(1);
    }
}

void initGenerator() {
    std::random_device device;
    generator.engine.seed(device());
}

template<typename linkType>
void insertRandom(BTree<RecordPhone, linkType>& tree, int countNodes) {
    while (countNodes != 0)
    {
        countNodes--;

        unsigned long long newKey = getRandomULL(89010000000, 89999999999);

        while(tree.findRecord(newKey))
            newKey = getRandomULL(89010000000, 89999999999);

        recordPhone record;

        std::string firstName = std::to_string(newKey);
        std::string secondName = std::to_string(newKey);

        strcpy(record.firstName, firstName.c_str());
        strcpy(record.secondName, secondName.c_str());
        record.age = getRandomInt(1, 100);
        record.telephone = newKey;

        RecordPhone data(record);
        tree.insert(data);
    }
}

template<typename linkType>
void insertRandom(BTree<RecordBirth, linkType>& tree, int countNodes) {
    while (countNodes != 0)
    {
        countNodes--;

        unsigned long long newKey = getRandomULL(89010000000, 89999999999);

        while(tree.findRecord(newKey))
            newKey = getRandomULL(89010000000, 89999999999);

        recordBirth record;

        std::string name = std::to_string(newKey);

        strcpy(record.name, name.c_str());
        record.birthYear = getRandomInt(1900, 2023);
        record.telephone = newKey;

        RecordBirth data(record);
        tree.insert(data);
    }
}


void printMenu()
{
    std::cout << " 1 - insert\n";
    std::cout << "10 - insert tree2\n";
    std::cout << " 2 - print records\n";
    std::cout << "20 - print records tree2\n";
    std::cout << " 3 - find record\n";
    std::cout << "30 - find record2\n";
    std::cout << " 4 - print tree\n";
    std::cout << "40 - print tree2\n";
    std::cout << " 5 - del key in tree\n";
    std::cout << "50 - del key in tree2\n";
    std::cout << " 6 - fill tree\n";
    std::cout << "60 - fill tree2\n";
    std::cout << " 7 - print count keys tree\n";
    std::cout << "70 - print count keys tree2\n";
    std::cout << " 0 - exit\n";
    std::cout << "cmd-> ";
}

int main()
{
    initGenerator();
    auto tree = BTree<RecordPhone, linkFS>();
    auto tree2 = BTree<RecordBirth, linkFS>();
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
            {
                recordPhone record;
                std::cout << "Insert First Name: ";
                std::cin >> record.firstName;
                std::cout << "Insert Second Name: ";
                std::cin >> record.secondName;
                std::cout << "Insert age: ";
                std::cin >> record.age;
                std::cout << "Insert telephone: ";
                std::cin >> record.telephone;
                RecordPhone data(record);
                ok = tree.insert(data);
                if (ok)
                    std::cout << "________INSERT_OK_________\n";
                else
                    std::cout << "______KEY_NOT_UNIQUE______\n";
                break;
            }
            case 10:
            {
                recordBirth record2;
                std::cout << "Insert Name: ";
                std::cin >> record2.name;
                std::cout << "Insert birth year: ";
                std::cin >> record2.birthYear;
                std::cout << "Insert telephone: ";
                std::cin >> record2.telephone;
                RecordBirth data2(record2);
                ok = tree2.insert(data2);
                if (ok)
                    std::cout << "________INSERT_OK_________\n";
                else
                    std::cout << "______KEY_NOT_UNIQUE______\n";
                break;
            }
            case 2:
                tree.printRecords();
                break;
            case 20:
                tree2.printRecords();
                break;
            case 3:
            {
                std::cout << "\nInsert key for found: ";
                std::cin >> findKey;
                auto data = tree.findRecord(findKey, true);
                std::cout << "___________RECORD__________\n";
                tree.printRecord(*data);
                std::cout << "___________________________\n";
                break;
            }
            case 30:
            {
                std::cout << "\nInsert key for found: ";
                std::cin >> findKey;
                auto data = tree2.findRecord(findKey, true);
                std::cout << "___________RECORD__________\n";
                tree2.printRecord(*data);
                std::cout << "___________________________\n";
                break;
            }
            case 4:
                tree.printTree();
                break;
            case 40:
                tree2.printTree();
                break;
            case 5:
            {
                unsigned long long delKey;
                std::cout << "\nInsert key for del: ";
                std::cin >> delKey;
                tree.del(delKey);
                break;
            }
            case 50:
            {
                unsigned long long delKey;
                std::cout << "\nInsert key for del: ";
                std::cin >> delKey;
                tree2.del(delKey);
                break;
            }
            case 6:
                std::cout << "\nInsert count nodes: ";
                std::cin >> countNodes;
                insertRandom(tree, countNodes);
                break;
            case 60:
                std::cout << "\nInsert count nodes: ";
                std::cin >> countNodes;
                insertRandom(tree2, countNodes);
                break;
            case 7:
                std::cout << "___________________________\n";
                std::cout << "Count keys: " << tree.countKeys() << std::endl;
                std::cout << "___________________________\n";
                break;
            case 70:
                std::cout << "___________________________\n";
                std::cout << "Count keys: " << tree2.countKeys() << std::endl;
                std::cout << "___________________________\n";
                break;
            default:
                break;
        }
        printMenu();
        std::cin >> i;
    }
    return 0;
}
