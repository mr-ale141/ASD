/*
    20. Составить   программу  поиска  записи  с  включением  в
сильно   ветвящемся   Б-дереве   порядка  N.  Б-дерево  должно
храниться и обрабатываться в файле с прямым доступом (12).
*/

#include "libs/headers/bPlusTree.hpp"
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
    char filaName[] = "1.bin";
    auto *record = new recordType;
    record->age = 32;
    record->telephone = 9366328474;
    strcpy(record->firstName, "name");
    strcpy(record->secondName, "surname");
    BPlusTree<recordType, decltype(record->telephone)> tree(3, filaName);
    tree.insert(record);
    tree.printTree();
    return 0;
}

// bool isLeaf = false;
// int N = _N;                    maxChilds
// int size = 0;                  maxKeys
// index parent = 0;
// index current = 0;
// keyType *keys = new keyType[N - 1];
// index *childs = new index[N];

// recordType *record;            record
