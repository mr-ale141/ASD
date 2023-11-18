/*
    20. Составить   программу  поиска  записи  с  включением  в
сильно   ветвящемся   Б-дереве   порядка  N.  Б-дерево  должно
храниться и обрабатываться в файле с прямым доступом (12).
*/

#include "libs/headers/b_tree.hpp"
#include "libs/headers/hash.hpp"
#include <cstring>

struct recordStruct
{
    char firstName[3 * 8];
    char secondName[3 * 8];
    unsigned long long age;
    unsigned long long telephone;
};

// struct Node
// {
//     bool isLeaf;
//     std::vector<unsigned long long> keys;
//     std::vector<Node> child;
// };

int main()
{
    // HashString hash;
    // hash.printPows();
    // unsigned long long hashStr;
    // std::string str = "Hello world!";
    // hashStr = hash.getHashStringByte(str);
    // std::cout << "\"" << str << "\" | len = " << str.length() << std::endl;
    // std::cout << hashStr << std::endl;
    // str = "HELLO WORLD!";
    // hashStr = hash.getHashStringByte(str);
    // std::cout << "\"" << str << "\" | len = " << str.length() << std::endl;
    // std::cout << hashStr << std::endl;
    // std::cout << sizeof(std::vector<unsigned long long>) << std::endl;
    // std::cout << sizeof(std::vector<Node>) << std::endl;
    // std::cout << sizeof(Node*) << std::endl;
    // std::cout << sizeof(bool) << std::endl;
    // while (str != "q")
    // {
    //     std::cout << "Insert string: ";
    //     std::cin >> str;
    //     hashStr = hash.getHashStringByte(str);
    //     std::cout << "\"" << str << "\" | len = " << str.length() << std::endl;
    //     std::cout << hashStr << std::endl;
    // }
    recordStruct record;
    strcpy(record.firstName, "name");
    strcpy(record.secondName, "surname");
    record.age = 32;
    record.telephone = 79236547;
    char filaName[] = "1.bin";
    BTree tree(3, filaName, record);
    return 0;
}
