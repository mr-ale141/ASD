#pragma once
#include "store.h"
#include "printHandler.h"

template <typename recordType, typename linkType = linkRAM<recordType>>
class BTree
{
    int N;
    Store<recordType, linkType> store;
    PrintHandler<recordType, linkType> printHandler;

public:
    BTree() : store(), printHandler(store) { N = store.N; }
    linkType findNodeInChildTree(keyType key, linkType& index);
    std::shared_ptr<recordType> findRecordInChildTree(keyType key, linkType& index, int count, bool withPrint);
    std::shared_ptr<recordType> findRecord(keyType key, bool withPrint = false);
    void insertRecord(linkType nodeLink, recordType& record, linkType child = linkType());
    linkType insertInChildTree(linkType& index, recordType& record);
    void createRootAndInsert(recordType& record, linkType childLeft = linkType(), linkType childRight = linkType());
    bool insert(recordType& record);
    recordType getRecordWithMaxKey(linkType& index);
    recordType getRecordWithMinKey(linkType& index);
    int delInLeaf(linkType& index, keyType delKey);
    void delKeyInChildTree(linkType& index, keyType delKey);
    void fixRules(linkType& index);
    void del(keyType delKey);
    int countKeysInNode(linkType& index);
    int countKeys();
    void printTree() { printHandler.printTree(); }
    void printRecords() { printHandler.printRecords(); }
    void printRecord(recordType& data) { printHandler.printRecord(data); }
};

template<typename recordType, typename linkType>
int BTree<recordType, linkType>::countKeys() {
    if (!store.root)
        return 0;
    else
        return countKeysInNode(store.root);
}

template<typename recordType, typename linkType>
int BTree<recordType, linkType>::countKeysInNode(linkType& index) {
    int countCurrent = 0;
    if (!index)
        return countCurrent;
    auto node = store.readNode(index);
    countCurrent += node->size;
    for (int i = 0; i < node->size; i++)
        countCurrent += countKeysInNode(node->children[i]);
    countCurrent += countKeysInNode(node->children[node->size]);
    return countCurrent;
}

template<typename recordType, typename linkType>
void BTree<recordType, linkType>::del(keyType delKey) {
    auto currentNode = store.readNode(store.root);
    if (currentNode->isLeaf)
    {
        auto node = findNodeInChildTree(delKey, store.root);
        if (!node)
            std::cout << "Key not found!!!!\n";
        else if (!delInLeaf(currentNode->current, delKey))
            store.reset();
    }
    else
        delKeyInChildTree(store.root, delKey);
}

template<typename recordType, typename linkType>
void BTree<recordType, linkType>::fixRules(linkType& index) {
    auto node = store.readNode(index);
    auto parentNode = store.readNode(node->parent);
    int i;
    for (i = 0; i < parentNode->size; i++)
        if (node->current == parentNode->children[i]) break;
    int indexParentKey = i;
    linkType brotherLeftLink;
    linkType brotherRightLink;
    if (indexParentKey == 0 || indexParentKey < parentNode->size)
        brotherRightLink = parentNode->children[i + 1];
    else
    {
        brotherLeftLink = parentNode->children[i - 1];
        indexParentKey--;
    }
    auto brotherLeft = store.readNode(brotherLeftLink);
    auto brotherRight = store.readNode(brotherRightLink);
    if (brotherRight && brotherRight->size >= N)
    {
        recordType recordFromBrother = brotherRight->data[0];
        delInLeaf(brotherRight->current, recordFromBrother.key);
        recordType recordParent = parentNode->data[indexParentKey];
        node->size++;
        node->keys[node->size - 1] = recordParent.key;
        node->data[node->size - 1] = recordParent;
        parentNode->keys[indexParentKey] = recordFromBrother.key;
        parentNode->data[indexParentKey] = recordFromBrother;
        store.writeNode(node->current, *node);
        store.writeNode(parentNode->current, *parentNode);
    }
    else if (brotherLeft && brotherLeft->size >= N)
    {
        recordType recordFromBrother = brotherLeft->data[brotherLeft->size - 1];
        delInLeaf(brotherLeft->current, recordFromBrother.key);
        recordType recordParent = parentNode->data[indexParentKey];
        node->size++;
        for (int j = node->size; j > 0; j--)
        {
            node->keys[j] = node->keys[j - 1];
            node->data[j] = node->data[j - 1];
        }
        node->keys[0] = recordParent.key;
        node->data[0] = recordParent;
        parentNode->keys[indexParentKey] = recordFromBrother.key;
        parentNode->data[indexParentKey] = recordFromBrother;
        store.writeNode(node->current, *node);
        store.writeNode(parentNode->current, *parentNode);
    }
    else
    {
        linkType indexNodeAfterSplit;
        if (brotherRight)
        {
            node->keys[node->size] = parentNode->keys[indexParentKey];
            node->data[node->size] = parentNode->data[indexParentKey];
            i = ++node->size;
            int j;
            for (j = 0; j < brotherRight->size; i++, j++)
            {
                node->keys[i] = brotherRight->keys[j];
                node->data[i] = brotherRight->data[j];
                node->children[i] = brotherRight->children[j];
            }
            node->children[i] = brotherRight->children[j];
            brotherRight->keys = std::move(node->keys);
            brotherRight->data = std::move(node->data);
            brotherRight->children = std::move(node->children);
            brotherRight->size = i;
            if (!brotherRight->isLeaf)
            {
                for (int j = 0; j <= brotherRight->size; j++)
                {
                    auto updateChildNode = store.readNode(brotherRight->children[j]);
                    if (updateChildNode)
                    {
                        updateChildNode->parent = brotherRight->current;
                        store.writeNode(updateChildNode->current, *updateChildNode);
                    }
                }
            }
            store.writeNode(brotherRight->current, *brotherRight);
            indexNodeAfterSplit = brotherRight->current;

            for (i = indexParentKey; i < parentNode->size; i++)
            {
                parentNode->keys[i] = parentNode->keys[i + 1];
                parentNode->children[i] = parentNode->children[i + 1];
                parentNode->data[i] = parentNode->data[i + 1];
            }
            parentNode->children[i] = parentNode->children[i + 1];
            parentNode->size--;
            store.writeNode(parentNode->current, *parentNode);
        }
        else if (brotherLeft)
        {
            brotherLeft->keys[brotherLeft->size] = parentNode->keys[indexParentKey];
            brotherLeft->data[brotherLeft->size] = parentNode->data[indexParentKey];
            i = ++brotherLeft->size;
            int j;
            for (j = 0; j < node->size; i++, j++)
            {
                brotherLeft->keys[i] = node->keys[j];
                brotherLeft->data[i] = node->data[j];
                brotherLeft->children[i] = node->children[j];
            }
            brotherLeft->children[i] = node->children[j];
            node->keys = std::move(brotherLeft->keys);
            node->data = std::move(brotherLeft->data);
            node->children = std::move(brotherLeft->children);
            node->size = i;
            if (!brotherLeft->isLeaf)
            {
                for (int j = 0; j <= brotherLeft->size; j++)
                {
                    auto updateChildNode = store.readNode(brotherLeft->children[j]);
                    if (updateChildNode)
                    {
                        updateChildNode->parent = brotherLeft->current;
                        store.writeNode(updateChildNode->current, *updateChildNode);
                    }
                }
            }
            store.writeNode(node->current, *node);
            indexNodeAfterSplit = node->current;

            for (i = indexParentKey; i < parentNode->size; i++)
            {
                parentNode->keys[i] = parentNode->keys[i + 1];
                parentNode->children[i] = parentNode->children[i + 1];
                parentNode->data[i] = parentNode->data[i + 1];
            }
            parentNode->children[i] = parentNode->children[i + 1];
            parentNode->size--;
            store.writeNode(parentNode->current, *parentNode);
        }
        if (parentNode->size < N - 1)
        {
            if (parentNode->current == store.root && parentNode->size == 0)
                store.setNewRootIndex(indexNodeAfterSplit);
            else if (parentNode->current != store.root)
                fixRules(parentNode->current);
        }
    }
}

template<typename recordType, typename linkType>
void BTree<recordType, linkType>::delKeyInChildTree(linkType& index, keyType delKey) {
    auto nodeLink = findNodeInChildTree(delKey, index);
    auto node = store.readNode(nodeLink);
    if (!node)
        std::cout << "Key not found!!!!\n";
    else if (node->isLeaf)
    {
        if (delInLeaf(node->current, delKey) < N-1)
            fixRules(node->current);
    }
    else
    {
        for (int i = 0; i < node->size; i++)
        {
            if (delKey == node->keys[i])
            {
                recordType record = getRecordWithMaxKey(node->children[i]);
                node->keys[i] = record.key;
                node->data[i] = record;
                store.writeNode(node->current, *node);
                delKeyInChildTree(node->children[i], record.key);
                break;
            }
        }
    }
}

template<typename recordType, typename linkType>
int BTree<recordType, linkType>::delInLeaf(linkType& index, keyType delKey) {
    auto node = store.readNode(index);
    int i;
    auto _keys = std::make_unique<keyType[]>(2 * N - 1);
    auto _children = std::make_unique<linkType[]>(2 * N);
    auto _data = std::make_unique<recordType[]>(2 * N - 1);
    for (i = 0; i < node->size; i++)
    {
        if (delKey > node->keys[i])
        {
            _keys[i] = node->keys[i];
            _children[i] = node->children[i];
            _data[i] = node->data[i];
        }
        else
            break;
    }
    node->size--;
    for (; i < node->size; i++)
    {
        _keys[i] = node->keys[i+1];
        _data[i] = node->data[i+1];
        _children[i] = node->children[i+1];
    }
    _children[i] = node->children[i+1];
    node->keys = std::move(_keys);
    node->children = std::move(_children);
    node->data = std::move(_data);
    store.writeNode(node->current, *node);
    int size = node->size;
    return size;
}

template<typename recordType, typename linkType>
recordType BTree<recordType, linkType>::getRecordWithMinKey(linkType& index) {
    auto node = store.readNode(index);
    linkType next;
    while(!node->isLeaf)
    {
        next = node->children[0];
        node = store.readNode(next);
    }
    recordType record = node->data[0];
    return record;
}

template<typename recordType, typename linkType>
recordType BTree<recordType, linkType>::getRecordWithMaxKey(linkType& index) {
    auto node = store.readNode(index);
    linkType next;
    while(!node->isLeaf)
    {
        next = node->children[node->size];
        node = store.readNode(next);
    }
    recordType record = node->data[node->size - 1];
    return record;
}

template<typename recordType, typename linkType>
bool BTree<recordType, linkType>::insert(recordType& record) {
    if (findRecord(record.key))
        return false;
    if (!store.root)
        createRootAndInsert(record);
    else
    {
        linkType nodeLink = insertInChildTree(store.root, record);
        auto node = store.readNode(nodeLink);
        if (node->size == (2 * N - 1))
        {
            int i;
            auto _keys = std::make_unique<keyType[]>(2 * N - 1);
            auto _children = std::make_unique<linkType[]>(2 * N);
            auto _data = std::make_unique<recordType[]>(2 * N - 1);
            auto newNode = store.getNewNode();
            newNode.isLeaf = node->isLeaf;
            newNode.parent = store.getAfterNextLink();
            newNode.current = store.getNextLink();
            newNode.size = N - 1;
            for (i = 0; i < (N - 1); i++)
            {
                newNode.keys[i] = node->keys[i];
                newNode.children[i] = node->children[i];
                newNode.data[i] = node->data[i];
            }
            newNode.children[i] = node->children[i];
            if (!newNode.isLeaf)
            {
                for (int j = 0; j <= newNode.size; j++)
                {
                    auto updateChildNode = store.readNode(newNode.children[j]);
                    if (updateChildNode)
                    {
                        updateChildNode->parent = newNode.current;
                        store.writeNode(updateChildNode->current, *updateChildNode);
                    }
                }
            }
            store.writeNode(newNode.current, newNode);
            recordType movedDataUp = node->data[i];
            createRootAndInsert(movedDataUp, newNode.current, node->current);
            int j = 0;
            for (i++; j < (N - 1); i++, j++)
            {
                _keys[j] = node->keys[i];
                _children[j] = node->children[i];
                _data[j] = node->data[i];
            }
            _children[j] = node->children[i];
            node->keys = std::move(_keys);
            node->children = std::move(_children);
            node->data = std::move(_data);
            node->size = N - 1;
            node->parent = store.root;
            store.writeNode(node->current, *node);
        }
    }
    return true;
}

template<typename recordType, typename linkType>
void BTree<recordType, linkType>::createRootAndInsert(recordType& record, linkType childLeft, linkType childRight)
{
    bool isLeaf;
    if (!store.root)
        isLeaf = true;
    else
        isLeaf = false;
    store.setNewRootIndex(store.getNextLink());
    auto newRoot = store.getNewNode();
    newRoot.isLeaf = isLeaf;
    newRoot.size = 1;
    newRoot.current = store.root;
    newRoot.keys[0] = record.key;
    newRoot.data[0] = record;
    newRoot.children[0] = childLeft;
    newRoot.children[1] = childRight;
    store.writeNode(newRoot.current, newRoot);
}

template<typename recordType, typename linkType>
linkType BTree<recordType, linkType>::insertInChildTree(linkType& link, recordType& record) {
    if (!link)
        return store.getZeroLink();
    auto node = store.readNode(link);
    if (!node->isLeaf)
    {
        linkType childNodeLink;
        int i;
        for (i = 0; i < node->size; i++)
        {
            if (record.key < node->keys[i])
            {
                linkType next = node->children[i];
                childNodeLink = insertInChildTree(next, record);
                break;
            }
        }
        if (i == node->size)
        {
            linkType next = node->children[node->size];
            childNodeLink = insertInChildTree(next, record);
        }
        auto childNode = store.readNode(childNodeLink);
        if (childNode && childNode->size == (2 * N - 1))
        {
            auto _keys = std::make_unique<keyType[]>(2 * N - 1);
            auto _children = std::make_unique<linkType[]>(2 * N);
            auto _data = std::make_unique<recordType[]>(2 * N - 1);
            auto newNode = store.getNewNode();
            newNode.isLeaf = childNode->isLeaf;
            newNode.parent = node->current;
            newNode.current = store.getNextLink();
            newNode.size = N - 1;
            for (i = 0; i < (N - 1); i++)
            {
                newNode.keys[i] = childNode->keys[i];
                newNode.children[i] = childNode->children[i];
                newNode.data[i] = childNode->data[i];
            }
            newNode.children[i] = childNode->children[i];
            recordType movedDataUp = childNode->data[i];
            insertRecord(node->current, movedDataUp, newNode.current);
            if (!newNode.isLeaf)
            {
                for (int j = 0; j <= newNode.size; j++)
                {
                    auto updateChildNode = store.readNode(newNode.children[i]);
                    if (updateChildNode)
                    {
                        updateChildNode->parent = newNode.current;
                        store.writeNode(updateChildNode->current, *updateChildNode);
                    }
                }
            }
            store.writeNode(newNode.current, newNode);
            int j = 0;
            for (i++; j < (N - 1); i++, j++)
            {
                _keys[j] = childNode->keys[i];
                _children[j] = childNode->children[i];
                _data[j] = childNode->data[i];
            }
            _children[j] = childNode->children[i];
            childNode->keys = std::move(_keys);
            childNode->children = std::move(_children);
            childNode->data = std::move(_data);
            childNode->size = N - 1;
            store.writeNode(childNode->current, *childNode);
        }
    }
    else
        insertRecord(node->current, record);
    return node->current;
}

template<typename recordType, typename linkType>
void BTree<recordType, linkType>::insertRecord(linkType nodeLink, recordType& record, linkType child) {
    int i;
    auto _keys = std::make_unique<keyType[]>(2 * N - 1);
    auto _children = std::make_unique<linkType[]>(2 * N);
    auto _data = std::make_unique<recordType[]>(2 * N - 1);
    auto node = store.readNode(nodeLink);
    for (i = 0; i < node->size; i++)
    {
        if (record.key > node->keys[i])
        {
            _keys[i] = node->keys[i];
            _children[i] = node->children[i];
            _data[i] = node->data[i];
        }
        else
            break;
    }
    _keys[i] = record.key;
    _children[i] = child;
    _data[i] = record;
    node->size++;
    for (i++; i < node->size; i++)
    {
        _keys[i] = node->keys[i-1];
        _data[i] = node->data[i-1];
        _children[i] = node->children[i-1];
    }
    _children[i] = node->children[i-1];
    node->keys = std::move(_keys);
    node->children = std::move(_children);
    node->data = std::move(_data);
    store.writeNode(node->current, *node);
}

template<typename recordType, typename linkType>
std::shared_ptr<recordType> BTree<recordType, linkType>::findRecord(keyType key, bool withPrint) {
    std::shared_ptr<recordType> record = nullptr;
    if (!store.root)
    {
        if (withPrint)
            std::cout << "________EMPTY_TREE________\n";
    }
    else
    {
        int count = 0;
        if (withPrint)
            std::cout << "_________FIND_HISTORY______\n";
        record = findRecordInChildTree(key, store.root, count, withPrint);
        if (withPrint)
            std::cout << "___________________________\n";
    }
    return record;
}

template<typename recordType, typename linkType>
std::shared_ptr<recordType> BTree<recordType, linkType>::findRecordInChildTree(keyType key, linkType& index, int count, bool withPrint) {
    if (!index)
        return nullptr;
    else if (withPrint)
        printHandler.printTab(count);
    std::shared_ptr<recordType> record;
    auto node = store.readNode(index);
    if (withPrint)
        printHandler.printNode(*node);
    count += SIZE_TAB;
    int i;
    for (i = 0; i < node->size; i++)
        if (key < node->keys[i])
        {
            record = findRecordInChildTree(key, node->children[i], count, withPrint);
            break;
        }
        else if (key == node->keys[i])
        {
            auto data = node->data[i];
            record = std::make_shared<recordType>(data);
            break;
        }
    if (i == node->size)
        record = findRecordInChildTree(key, node->children[node->size], count, withPrint);
    return record;
}

template<typename recordType, typename linkType>
linkType BTree<recordType, linkType>::findNodeInChildTree(keyType key, linkType& index) {
    if (!index)
        return store.getZeroLink();
    auto node = store.readNode(index);
    linkType next;
    for (int i = 0; i < node->size; i++)
        if (key < node->keys[i])
        {
            next = node->children[i];
            return findNodeInChildTree(key, next);
        }
        else if (key == node->keys[i])
            return node->current;
    next = node->children[node->size];
    return findNodeInChildTree(key, next);
}
