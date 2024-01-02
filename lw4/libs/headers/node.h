#pragma once
#include <memory>
#include "record.h"

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
    : isLeaf(false), N(treeN), size(0)
    {
        keys = std::make_unique<keyType[]>(2 * N - 1);
        data = std::make_unique<recordType[]>(2 * N - 1);
    }
};

template <typename recordType>
class NodeRAM : public Node<recordType>
{
public:
    std::shared_ptr<NodeRAM<recordType>> parent;
    std::shared_ptr<NodeRAM<recordType>> current;
    std::unique_ptr<std::shared_ptr<NodeRAM<recordType>>[]> children;

    explicit NodeRAM(int treeN)
    : Node<recordType>(treeN), parent(nullptr), current(nullptr)
    {
        children = std::make_unique<std::shared_ptr<NodeRAM<recordType>>[]>(2 * treeN);
    }
};

template <typename recordType>
class NodeFS : public Node<recordType>
{
public:
    unsigned long long parent;
    unsigned long long current;
    std::unique_ptr<unsigned long long[]> children;

    explicit NodeFS(int treeN)
    : Node<recordType>(treeN), parent(0ULL), current(0ULL)
    {
        children = std::make_unique<unsigned long long[]>(2 * treeN);
    }
};