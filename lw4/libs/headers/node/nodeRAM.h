#pragma once
#include "node.h"

template <typename recordType>
class NodeRAM : public Node<recordType> {
public:
    std::unique_ptr<Node<recordType>> parent;
    std::unique_ptr<Node<recordType>> current;
    std::unique_ptr<Node<recordType>[]> children;

    explicit NodeRAM(int treeN)
    : Node<recordType>(treeN)
    {
        parent = nullptr;
        current = nullptr;
        children = std::make_unique<Node<recordType>[]>(2 * treeN);
    }
};