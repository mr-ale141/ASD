#pragma once
#include "node.h"

typedef unsigned long long indexType;

template <typename recordType>
class NodeFS : public Node<recordType> {
public:
    indexType parent;
    indexType current;
    std::unique_ptr<indexType[]> children;

    explicit NodeFS(int treeN)
    : Node<recordType>(treeN)
    {
        parent = 0ULL;
        current = 0ULL;
        children = std::make_unique<indexType[]>(2 * treeN);
    }
};