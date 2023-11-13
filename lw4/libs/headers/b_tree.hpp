#pragma once
#include <iostream>
#include <fstream>

struct recordStruct
{
    char firstName[3 * 8];
    char secondName[3 * 8];
    unsigned long long age;
    unsigned long long telephone;
};

class BTree
{
public:
    BTree(int N, char* file_name, recordStruct &firstData);
    int ins(char* str);
    void del(char *str);
    unsigned long long find(char *str);
private:
    std::fstream file;
    struct NodeStruct
    {
        unsigned long long isLeaf;
        unsigned long long keys[3];
        unsigned long long childs[3];
    };
    struct BTreeStruct {
        int N;
        NodeStruct root;
    } BTreeRoot;
};
