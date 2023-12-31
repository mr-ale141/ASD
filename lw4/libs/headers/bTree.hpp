#pragma once
#include "store.h"
#include "printHandler.h"

template <typename recordType, typename linkType>
class BTree
{
    int N;
    Store<recordType, linkType> store;
    PrintHandler<recordType, linkType> printHandler;

public:
    BTree();
    std::shared_ptr<Node<recordType, linkType>> findNodeInChildTree(keyType key, linkType& index);
    recordType findRecordInChildTree(keyType key, linkType& index, int count, bool withPrint);
    recordType findRecord(keyType key, bool withPrint = false);
    void insertRecord(Node<recordType, keyType>& node, recordType& record, linkType& child = 0);
    std::shared_ptr<Node<recordType, linkType>> insertInChildTree(linkType& index, recordType& record);
    void createRootAndInsert(recordType& record, linkType& childLeft = 0, linkType& childRight = 0);
    bool insert(recordType& record);
    recordType getRecordWithMaxKey(linkType& index);
    recordType getRecordWithMinKey(linkType& index);
    int delInLeaf(linkType& index, keyType delKey);
    void delKeyInChildTree(linkType& index, keyType delKey);
    void fixRules(linkType& index);
    void del(keyType delKey);
    int countKeysInNode(linkType& index);
    int countKeys();
    void printTree() {printHandler.printTree();}
    void printRecords() {printHandler.printRecords();}
    void printRecord(recordType& record) {printHandler.printRecord(record);}
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
    countCurrent += node.size;
    for (int i = 0; i < node.size; i++)
        countCurrent += countKeysInNode(node.children[i]);
    countCurrent += countKeysInNode(node.children[node.size]);
    return countCurrent;
}

template<typename recordType, typename linkType>
void BTree<recordType, linkType>::del(keyType delKey) {
    auto currentNode = store.readNode(store.root);
    if (currentNode.isLeaf)
    {
        auto node = findNodeInChildTree(delKey, store.root);
        if (!node)
            std::cout << "Key not found!!!!\n";
        else if (!delInLeaf(currentNode.current, delKey))
        {
            store.file.close();
            store.createNewFile(N, store.fileName);
        }
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
    std::shared_ptr<Node<recordType, linkType>> brotherLeft = nullptr;
    std::shared_ptr<Node<recordType, linkType>> brotherRight = nullptr;
    if (indexParentKey == 0 || indexParentKey < parentNode->size)
        brotherRight = store.readNode(parentNode->children[i + 1]);
    else
    {
        brotherLeft = store.readNode(parentNode->children[i - 1]);
        indexParentKey--;
    }
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
        store.writeNode(node->current, node);
        store.writeNode(parentNode->current, parentNode);
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
        store.writeNode(node->current, node);
        store.writeNode(parentNode->current, parentNode);
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
            *(brotherRight->keys) = *(node->keys);
            *(brotherRight->data) = *(node->data);
            *(brotherRight->children) = *(node->children);
            brotherRight->size = i;
            if (!brotherRight->isLeaf)
            {
                for (int j = 0; j <= brotherRight->size; j++)
                {
                    auto updateChildNode = store.readNode(brotherRight->children[j]);
                    updateChildNode->parent = brotherRight->current;
                    store.writeNode(updateChildNode->current, updateChildNode);
                }
            }
            store.writeNode(brotherRight->current, brotherRight);
            indexNodeAfterSplit = brotherRight->current;

            for (i = indexParentKey; i < parentNode->size; i++)
            {
                parentNode->keys[i] = parentNode->keys[i + 1];
                parentNode->children[i] = parentNode->children[i + 1];
                parentNode->data[i] = parentNode->data[i + 1];
            }
            parentNode->children[i] = parentNode->children[i + 1];
            parentNode->size--;
            store.writeNode(parentNode->current, parentNode);
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
            *(node->keys) = *(brotherLeft->keys);
            *(node->data) = *(brotherLeft->data);
            *(node->children) = *(brotherLeft->children);
            node->size = i;
            if (!brotherLeft->isLeaf)
            {
                for (int j = 0; j <= brotherLeft->size; j++)
                {
                    auto updateChildNode = store.readNode(brotherLeft->children[j]);
                    updateChildNode->parent = brotherLeft->current;
                    store.writeNode(updateChildNode->current, updateChildNode);
                }
            }
            store.writeNode(node->current, node);
            indexNodeAfterSplit = node->current;

            for (i = indexParentKey; i < parentNode->size; i++)
            {
                parentNode->keys[i] = parentNode->keys[i + 1];
                parentNode->children[i] = parentNode->children[i + 1];
                parentNode->data[i] = parentNode->data[i + 1];
            }
            parentNode->children[i] = parentNode->children[i + 1];
            parentNode->size--;
            store.writeNode(parentNode->current, parentNode);
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
    auto node = findNodeInChildTree(delKey, index);
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
                store.writeNode(node->current, node);
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
    node->keys = _keys;
    node->children = _children;
    node->data = _data;
    store.writeNode(node->current, node);
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
        auto node = insertInChildTree(store.root, record);
        if (node->size == (2 * N - 1))
        {
            int i;
            auto _keys = std::make_unique<keyType[]>(2 * N - 1);
            auto _children = std::make_unique<linkType[]>(2 * N);
            auto _data = std::make_unique<recordType[]>(2 * N - 1);
            auto newNode = Node<recordType, keyType>(N);
            newNode.isLeaf = node->isLeaf;
            newNode.parent = store->writeIndex + 1;
            newNode.current = store->writeIndex;
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
                    updateChildNode->parent = newNode.current;
                    store.writeNode(updateChildNode->current, *updateChildNode);
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
            node->keys = _keys;
            node->children = _children;
            node->data = _data;
            node->size = N - 1;
            node->parent = store.root;
            store.writeNode(node->current, *node);
        }
    }
    return true;
}

template<typename recordType, typename linkType>
void BTree<recordType, linkType>::createRootAndInsert(recordType& record, linkType& childLeft, linkType& childRight) {
    bool isLeaf;
    if (!store.root)
        isLeaf = true;
    else
        isLeaf = false;
    store.setNewRootIndex(store.writeIndex);
    auto newRoot = Node<recordType, keyType>(N);
    newRoot.isLeaf = isLeaf;
    newRoot.size = 1;
    newRoot.current = store->rootIndex;
    newRoot.keys[0] = record->telephone;
    newRoot.data[0] = *record;
    newRoot.children[0] = childLeft;
    newRoot.children[1] = childRight;
    store.writeNode(newRoot.current, newRoot);
}

template<typename recordType, typename linkType>
std::shared_ptr<Node<recordType, linkType>> BTree<recordType, linkType>::insertInChildTree(linkType& index, recordType& record) {
    if (!index)
        return nullptr;
    auto node = store.readNode(index);
    if (!node->isLeaf)
    {
        auto childNode = std::make_shared<Node<recordType, linkType>>(N);
        int i;
        for (i = 0; i < node->size; i++)
        {
            if (record.key < node->keys[i])
            {
                linkType next = node->children[i];
                childNode = insertInChildTree(next, record);
                break;
            }
        }
        if (i == node->size)
        {
            linkType next = node->children[node->size];
            childNode = insertInChildTree(next, record);
        }
        if (childNode && childNode->size == (2 * N - 1))
        {
            auto _keys = std::make_unique<keyType[]>(2 * N - 1);
            auto _children = std::make_unique<linkType[]>(2 * N);
            auto _data = std::make_unique<recordType[]>(2 * N - 1);
            auto newNode = Node<recordType, keyType>(N);
            newNode.isLeaf = childNode->isLeaf;
            newNode.parent = node->current;
            newNode.current = store.writeIndex;
            newNode.size = N - 1;
            for (i = 0; i < (N - 1); i++)
            {
                newNode.keys[i] = childNode->keys[i];
                newNode.children[i] = childNode->children[i];
                newNode.data[i] = childNode->data[i];
            }
            newNode.children[i] = childNode->children[i];
            recordType movedDataUp = childNode->data[i];
            insertRecord(*node, movedDataUp, newNode.current);
            if (!newNode.isLeaf)
            {
                for (int j = 0; j <= newNode.size; j++)
                {
                    auto updateChildNode = store.readNode(newNode.children[i]);
                    updateChildNode->parent = newNode.current;
                    store.writeNode(updateChildNode->current, *updateChildNode);
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
            childNode->keys = _keys;
            childNode->children = _children;
            childNode->data = _data;
            childNode->size = N - 1;
            store.writeNode(childNode->current, *childNode);
        }
    }
    else
        insertRecord(*node, record);
    store.writeNode(node->current, *node);
    return node;
}

template<typename recordType, typename linkType>
void BTree<recordType, linkType>::insertRecord(Node<recordType, keyType>& node, recordType& record, linkType& child) {
    int i;
    auto _keys = std::make_unique<keyType[]>(2 * N - 1);
    auto _children = std::make_unique<linkType[]>(2 * N);
    auto _data = std::make_unique<recordType[]>(2 * N - 1);
    for (i = 0; i < node.size; i++)
    {
        if (record.key > node.keys[i])
        {
            _keys[i] = node.keys[i];
            _children[i] = node.children[i];
            _data[i] = node.data[i];
        }
        else
            break;
    }
    _keys[i] = record.key;
    _children[i] = child;
    _data[i] = record;
    node.size++;
    for (i++; i < node.size; i++)
    {
        _keys[i] = node.keys[i-1];
        _data[i] = node.data[i-1];
        _children[i] = node.children[i-1];
    }
    _children[i] = node.children[i-1];
    node.keys = _keys;
    node.children = _children;
    node.data = _data;
}

template<typename recordType, typename linkType>
recordType BTree<recordType, linkType>::findRecord(keyType key, bool withPrint) {
    recordType record;
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
recordType BTree<recordType, linkType>::findRecordInChildTree(keyType key, linkType& index, int count, bool withPrint) {
    if (!index)
        return nullptr;
    else if (withPrint)
        printHandler.printTab(count);
    recordType record;
    auto node = store.readNode(index);
    if (withPrint)
        printHandler.printNode(node);
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
            record = node->data[i];
            break;
        }
    if (i == node->size)
        record = findRecordInChildTree(key, node->children[node->size], count, withPrint);
    return record;
}

template<typename recordType, typename linkType>
std::shared_ptr<Node<recordType, linkType>> BTree<recordType, linkType>::findNodeInChildTree(keyType key, linkType& index) {
    if (!index)
        return nullptr;
    auto node = store.readNode(index);
    linkType next;
    for (int i = 0; i < node->size; i++)
        if (key < node->keys[i])
        {
            next = node->children[i];
            return findNodeInChildTree(key, next);
        }
        else if (key == node->keys[i])
            return node;
    next = node->children[node->size];
    return findNodeInChildTree(key, next);
}

template <typename recordType, typename linkType>
BTree<recordType, linkType>::BTree()
{
    store = Store<recordType, linkType>();
    N = store.N;
    printHandler = PrintHandler<recordType, linkType>(store);
}