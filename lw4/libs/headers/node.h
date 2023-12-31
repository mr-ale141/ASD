#pragma once
#include <memory>
#include "record.h"

typedef unsigned long long linkFS;
typedef std::shared_ptr<void> linkRAM;

template <typename recordType, typename linkType>
class Node
{
public:
    bool isLeaf;
    int N;
    int size;
    std::unique_ptr<keyType[]> keys;
    std::unique_ptr<recordType[]> data;

    std::weak_ptr<Node<recordType, linkType>> parent;
    std::weak_ptr<Node<recordType, linkType>> current;
    std::unique_ptr<linkType[]> children;

    explicit Node(int treeN)
    : isLeaf(false), N(treeN), size(0), parent(nullptr), current(nullptr)
    {
        keys = std::make_unique<keyType[]>(2 * N - 1);
        data = std::make_unique<recordType[]>(2 * N - 1);
        children = std::make_unique<linkType[]>(2 * N);
    }
};

template <typename recordType>
class Node<recordType, linkFS>
{
public:
    bool isLeaf;
    int N;
    int size;
    std::unique_ptr<keyType[]> keys;
    std::unique_ptr<recordType[]> data;

    linkFS parent;
    linkFS current;
    std::unique_ptr<linkFS[]> children;

    explicit Node(int treeN)
    : isLeaf(false), N(treeN), size(0), parent(0ULL), current(0ULL)
    {
        keys = std::make_unique<keyType[]>(2 * N - 1);
        data = std::make_unique<recordType[]>(2 * N - 1);
        children = std::make_unique<linkFS[]>(2 * N);
    }
};