/*
    20. Составить   программу  поиска  записи  с  включением  в
сильно   ветвящемся   Б-дереве   порядка  N.  Б-дерево  должно
храниться и обрабатываться в файле с прямым доступом (12).
*/

// #include "libs/headers/b_tree.hpp"
#include "libs/headers/sha512.hh"
#include <iostream>

int main()
{
    auto hash = sw::sha512::calculate("Исходник");
    std::cout << hash << std::endl;
    return 0;
}

