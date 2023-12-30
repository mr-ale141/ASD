#pragma once
#include <memory>
#include "../record/record.h"

template <typename recordType>
class Node
{
public:
    bool isLeaf;
    int N;
    int size;
    std::unique_ptr<keyType[]> keys;
    std::unique_ptr<recordType[]> data;

    explicit Node(int treeN)
    {
        isLeaf = false;
        N = treeN;
        size = 0;

        keys = std::make_unique<keyType[]>(2 * N - 1);

        data = std::make_unique<recordType[]>(2 * N - 1);
    }
};