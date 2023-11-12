/*
    20. Составить   программу  поиска  записи  с  включением  в
сильно   ветвящемся   Б-дереве   порядка  N.  Б-дерево  должно
храниться и обрабатываться в файле с прямым доступом (12).
*/

// #include "libs/headers/b_tree.hpp"
#include "libs/headers/hash.hpp"

int main()
{
    // HashString hash();
    HashString* hash = new HashString();
    hash->printPows();
    unsigned long long hashStr;
    std::string str = "Hello world!";
    hashStr = hash->getHashString(str);
    std::cout << "\"" << str << "\" | len = " << str.length() << std::endl;
    std::cout << hashStr << std::endl;
    str = "HELLO WORLD!";
    hashStr = hash->getHashString(str);
    std::cout << "\"" << str << "\" | len = " << str.length() << std::endl;
    std::cout << hashStr << std::endl;
    while (str != "q")
    {
        std::cout << "Insert string: ";
        std::cin >> str;
        hashStr = hash->getHashString(str);
        std::cout << "\"" << str << "\" | len = " << str.length() << std::endl;
        std::cout << hashStr << std::endl;
    }
    return 0;
}
