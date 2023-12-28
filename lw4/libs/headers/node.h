#pragma once

typedef unsigned long long indexType;

template <typename recordType, typename keyType>
class Node
{
public:
    bool isLeaf;
    int N;
    int size;
    indexType parent;
    indexType current;
    keyType* keys;
    indexType* children;
    recordType* data;

    explicit Node(int treeN)
    {
        isLeaf = false;
        N = treeN;
        size = 0;
        parent = 0;
        current = 0;

        keys = new keyType[2 * N - 1];
        for (int i = 0; i < (2 * N - 1); i++)
            keys[i] = 0;

        children = new indexType[2 * N];
        for (int i = 0; i < (2 * N); i++)
            children[i] = 0;

        data = new recordType[2 * N - 1]{};
    }

    ~Node()
    {
        delete[] keys;
        delete[] children;
        delete[] data;
    }
};