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
    recordType *record = new recordType;
    record->age = 32;
    record->telephone = 9266328474;
    strcpy(record->firstName, "name");
    strcpy(record->secondName, "surname");
    BPlusTree<recordType, decltype(record->telephone)> tree(3, filaName);
    tree.insert(record);
    return 0;
}

// bool isLeaf;
// int N;
// int size;
// index parent;
// index next;
// index prev;
// recordType *record;
// keyType *keys;
// index *childs;