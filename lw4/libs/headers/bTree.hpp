#pragma once
#include "fileHandler.h"
#include "printHandler.h"
#include "node.h"

template <typename recordType, typename keyType>
class BTree
{
    int N;
    struct PRNG
    {
        std::mt19937 engine;
    };
    PRNG generator;
    FileHandler<recordType, keyType>* fileHandler;
    PrintHandler<recordType, keyType>* printHandler;

public:
    BTree();
    ~BTree();
    Node<recordType, keyType>* findNodeInChildTree(keyType key, indexType index);
    recordType* findRecordInChildTree(keyType key, indexType index, int count, bool withPrint);
    recordType* findRecord(keyType key, bool withPrint = false);
    void insertRecord(Node<recordType, keyType>* node, recordType* record, indexType child = 0);
    Node<recordType, keyType>* insertInChildTree(indexType index, recordType* record);
    void createRootAndInsert(recordType* record, indexType childLeft = 0, indexType childRight = 0);
    bool insert(recordType* record);
    void initGenerator();
    unsigned long long getRandomULL(unsigned long long minValue, unsigned long long maxValue);
    unsigned getRandomInt(unsigned minValue, unsigned maxValue);
    void insertRandom(int countNodes);
    recordType* getRecordWithMaxKey(indexType index);
    recordType* getRecordWithMinKey(indexType index);
    int delInLeaf(indexType index, keyType delKey);
    void delKeyInChildTree(indexType index, keyType delKey);
    void fixRules(indexType index);
    void del(keyType delKey);
    void printTree() {printHandler->printTree();}
    void printRecords() {printHandler->printRecords();}
    int countKeys() {return printHandler->countKeys();}
    void printRecord(recordType* record) {printHandler->printRecord(record);}
};

template<typename recordType, typename keyType>
void BTree<recordType, keyType>::del(keyType delKey) {
    Node<recordType, keyType>* currentNode;
    currentNode = fileHandler->readNode(fileHandler->rootIndex);
    if (currentNode->isLeaf)
    {
        Node<recordType, keyType>* node = findNodeInChildTree(delKey, fileHandler->rootIndex);
        if (!node)
            std::cout << "Key not found!!!!\n";
        else if (!delInLeaf(currentNode->current, delKey))
        {
            fileHandler->file.close();
            fileHandler->createNewFile(N, fileHandler->fileName);
        }
        delete node;
    }
    else
    {
        delKeyInChildTree(fileHandler->rootIndex, delKey);
    }
    delete currentNode;
}

template<typename recordType, typename keyType>
void BTree<recordType, keyType>::fixRules(indexType index) {
    Node<recordType, keyType>* node = fileHandler->readNode(index);
    Node<recordType, keyType>* parentNode = fileHandler->readNode(node->parent);
    int i;
    for (i = 0; i < parentNode->size; i++)
    {
        if (node->current == parentNode->children[i])
            break;
    }
    indexType indexParentKey = i;
    Node<recordType, keyType>* brotherLeft = nullptr;
    Node<recordType, keyType>* brotherRight = nullptr;
    if (indexParentKey == 0 || indexParentKey < parentNode->size)
    {
        brotherRight = fileHandler->readNode(parentNode->children[i + 1]);
    }
    else
    {
        brotherLeft = fileHandler->readNode(parentNode->children[i - 1]);
        indexParentKey--;
    }
    if (brotherRight && brotherRight->size >= N)
    {
        recordType recordFromBrother = brotherRight->data[0];
        delInLeaf(brotherRight->current, recordFromBrother.telephone);
        recordType recordParent = parentNode->data[indexParentKey];
        node->size++;
        node->keys[node->size - 1] = recordParent.telephone;
        node->data[node->size - 1] = recordParent;
        parentNode->keys[indexParentKey] = recordFromBrother.telephone;
        parentNode->data[indexParentKey] = recordFromBrother;
        fileHandler->writeNode(node->current, node);
        fileHandler->writeNode(parentNode->current, parentNode);
        delete brotherRight;
    }
    else if (brotherLeft && brotherLeft->size >= N)
    {
        recordType recordFromBrother = brotherLeft->data[brotherLeft->size - 1];
        delInLeaf(brotherLeft->current, recordFromBrother.telephone);
        recordType recordParent = parentNode->data[indexParentKey];
        node->size++;
        for (int j = node->size; j > 0; j--)
        {
            node->keys[j] = node->keys[j - 1];
            node->data[j] = node->data[j - 1];
        }
        node->keys[0] = recordParent.telephone;
        node->data[0] = recordParent;
        parentNode->keys[indexParentKey] = recordFromBrother.telephone;
        parentNode->data[indexParentKey] = recordFromBrother;
        fileHandler->writeNode(node->current, node);
        fileHandler->writeNode(parentNode->current, parentNode);
        delete brotherLeft;
    }
    else
    {
        indexType indexNodeAfterSplit;
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
                    Node<recordType, keyType>* updateChildNode = fileHandler->readNode(brotherRight->children[j]);
                    updateChildNode->parent = brotherRight->current;
                    fileHandler->writeNode(updateChildNode->current, updateChildNode);
                    delete updateChildNode;
                }
            }
            fileHandler->writeNode(brotherRight->current, brotherRight);
            indexNodeAfterSplit = brotherRight->current;

            for (i = indexParentKey; i < parentNode->size; i++)
            {
                parentNode->keys[i] = parentNode->keys[i + 1];
                parentNode->children[i] = parentNode->children[i + 1];
                parentNode->data[i] = parentNode->data[i + 1];
            }
            parentNode->children[i] = parentNode->children[i + 1];
            parentNode->size--;
            fileHandler->writeNode(parentNode->current, parentNode);

            delete brotherRight;
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
                    Node<recordType, keyType>* updateChildNode = fileHandler->readNode(brotherLeft->children[j]);
                    updateChildNode->parent = brotherLeft->current;
                    fileHandler->writeNode(updateChildNode->current, updateChildNode);
                    delete updateChildNode;
                }
            }
            fileHandler->writeNode(node->current, node);
            indexNodeAfterSplit = node->current;

            for (i = indexParentKey; i < parentNode->size; i++)
            {
                parentNode->keys[i] = parentNode->keys[i + 1];
                parentNode->children[i] = parentNode->children[i + 1];
                parentNode->data[i] = parentNode->data[i + 1];
            }
            parentNode->children[i] = parentNode->children[i + 1];
            parentNode->size--;
            fileHandler->writeNode(parentNode->current, parentNode);

            delete brotherLeft;
        }
        if (parentNode->size < N - 1)
        {
            if (parentNode->current == fileHandler->rootIndex && parentNode->size == 0)
            {
                fileHandler->setNewRootIndex(indexNodeAfterSplit);
            }
            else if (parentNode->current != fileHandler->rootIndex)
            {
                fixRules(parentNode->current);
            }
        }
    }
    delete parentNode;
    delete node;
}

template<typename recordType, typename keyType>
void BTree<recordType, keyType>::delKeyInChildTree(indexType index, keyType delKey) {
    Node<recordType, keyType>* node = findNodeInChildTree(delKey, index);
    if (!node)
        std::cout << "Key not found!!!!\n";
    else if (node->isLeaf)
    {
        if (delInLeaf(node->current, delKey) < N-1)
        {
            fixRules(node->current);
        }
    }
    else
    {
        for (int i = 0; i < node->size; i++)
        {
            if (delKey == node->keys[i])
            {
                recordType record = *getRecordWithMaxKey(node->children[i]);
                node->keys[i] = record.telephone;
                node->data[i] = record;
                fileHandler->writeNode(node->current, node);
                delKeyInChildTree(node->children[i], record.telephone);
                break;
            }
        }
    }
    delete node;
}

template<typename recordType, typename keyType>
int BTree<recordType, keyType>::delInLeaf(indexType index, keyType delKey) {
    Node<recordType, keyType>* node = fileHandler->readNode(index);
    int i;
    auto *_keys = new keyType[2 * N - 1];
    auto *_children = new indexType[2 * N];
    auto *_data = new recordType[2 * N - 1]{};
    for (i = 0; i < (2 * N - 1); i++)
        _keys[i] = 0;
    for (i = 0; i < (2 * N); i++)
        _children[i] = 0;
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
    delete[] node->keys;
    delete[] node->children;
    delete[] node->data;
    node->keys = _keys;
    node->children = _children;
    node->data = _data;
    fileHandler->writeNode(node->current, node);
    int size = node->size;
    delete node;
    return size;
}

template<typename recordType, typename keyType>
recordType *BTree<recordType, keyType>::getRecordWithMinKey(indexType index) {
    Node<recordType, keyType>* node = fileHandler->readNode(index);
    indexType next;
    while(!node->isLeaf)
    {
        next = node->children[0];
        delete node;
        node = fileHandler->readNode(next);
    }
    recordType* record = &(node->data[0]);
    delete node;
    return record;
}

template<typename recordType, typename keyType>
recordType *BTree<recordType, keyType>::getRecordWithMaxKey(indexType index) {
    Node<recordType, keyType>* node = fileHandler->readNode(index);
    indexType next;
    while(!node->isLeaf)
    {
        next = node->children[node->size];
        delete node;
        node = fileHandler->readNode(next);
    }
    recordType* record = &(node->data[node->size - 1]);
    delete node;
    return record;
}

template<typename recordType, typename keyType>
void BTree<recordType, keyType>::insertRandom(int countNodes) {
    while (countNodes != 0)
    {
        countNodes--;
        unsigned long long newKey = getRandomULL(89010000000, 89999999999);
        while(findRecord(newKey))
            newKey = getRandomULL(89010000000, 89999999999);
        recordType* record = new recordType{};
        std::string firstName = std::to_string(newKey);
        std::string secondName = std::to_string(newKey);
        strcpy(record->firstName, firstName.c_str());
        strcpy(record->secondName, secondName.c_str());
        record->age = getRandomInt(1, 100);
        record->telephone = newKey;
        insert(record);
        delete record;
    }
}

template<typename recordType, typename keyType>
bool BTree<recordType, keyType>::insert(recordType *record) {
    if (findRecord(record->telephone))
        return false;
    if (fileHandler->rootIndex == 0)
        createRootAndInsert(record);
    else
    {
        Node<recordType, keyType>* node = insertInChildTree(fileHandler->rootIndex, record);
        if (node->size == (2 * N - 1))
        {
            int i;
            auto *_keys = new keyType[2 * N - 1];
            auto *_children = new indexType[2 * N];
            auto *_data = new recordType[2 * N - 1]{};
            for (i = 0; i < (2 * N - 1); i++)
                _keys[i] = 0;
            for (i = 0; i < (2 * N); i++)
                _children[i] = 0;
            auto* newNode = new Node<recordType, keyType>(N);
            newNode->isLeaf = node->isLeaf;
            newNode->parent = fileHandler->writeIndex + 1;
            newNode->current = fileHandler->writeIndex;
            newNode->size = N - 1;
            for (i = 0; i < (N - 1); i++)
            {
                newNode->keys[i] = node->keys[i];
                newNode->children[i] = node->children[i];
                newNode->data[i] = node->data[i];
            }
            newNode->children[i] = node->children[i];
            if (!newNode->isLeaf)
            {
                for (int j = 0; j <= newNode->size; j++)
                {
                    Node<recordType, keyType>* updateChildNode = fileHandler->readNode(newNode->children[j]);
                    updateChildNode->parent = newNode->current;
                    fileHandler->writeNode(updateChildNode->current, updateChildNode);
                    delete updateChildNode;
                }
            }
            fileHandler->writeNode(newNode->current, newNode);
            recordType* movedDataUp = &node->data[i];
            createRootAndInsert(movedDataUp, newNode->current, node->current);
            delete newNode;
            int j = 0;
            for (i++; j < (N - 1); i++, j++)
            {
                _keys[j] = node->keys[i];
                _children[j] = node->children[i];
                _data[j] = node->data[i];
            }
            _children[j] = node->children[i];
            delete[] node->keys;
            delete[] node->children;
            delete[] node->data;
            node->keys = _keys;
            node->children = _children;
            node->data = _data;
            node->size = N - 1;
            node->parent = fileHandler->rootIndex;
            fileHandler->writeNode(node->current, node);
        }
        delete node;
    }
    return true;
}

template<typename recordType, typename keyType>
void BTree<recordType, keyType>::createRootAndInsert(recordType *record, indexType childLeft, indexType childRight) {
    bool isLeaf;
    if (!fileHandler->rootIndex)
        isLeaf = true;
    else
        isLeaf = false;
    fileHandler->setNewRootIndex(fileHandler->writeIndex);
    auto* newRoot = new Node<recordType, keyType>(N);
    newRoot->isLeaf = isLeaf;
    newRoot->size = 1;
    newRoot->current = fileHandler->rootIndex;
    newRoot->keys[0] = record->telephone;
    newRoot->data[0] = *record;
    newRoot->children[0] = childLeft;
    newRoot->children[1] = childRight;
    fileHandler->writeNode(newRoot->current, newRoot);
    delete newRoot;
}

template<typename recordType, typename keyType>
Node<recordType, keyType> *BTree<recordType, keyType>::insertInChildTree(indexType index, recordType *record) {
    if (!index)
        return nullptr;
    Node<recordType, keyType>* node = fileHandler->readNode(index);
    if (!node->isLeaf)
    {
        Node<recordType, keyType>* childNode;
        int i;
        for (i = 0; i < node->size; i++)
        {
            if (record->telephone < node->keys[i])
            {
                indexType next = node->children[i];
                childNode = insertInChildTree(next, record);
                break;
            }
        }
        if (i == node->size)
        {
            indexType next = node->children[node->size];
            childNode = insertInChildTree(next, record);
        }
        if (childNode && childNode->size == (2 * N - 1))
        {
            auto *_keys = new keyType[2 * N - 1];
            auto *_children = new indexType[2 * N];
            auto *_data = new recordType[2 * N - 1]{};
            for (i = 0; i < (2 * N - 1); i++)
                _keys[i] = 0;
            for (i = 0; i < (2 * N); i++)
                _children[i] = 0;
            auto* newNode = new Node<recordType, keyType>(N);
            newNode->isLeaf = childNode->isLeaf;
            newNode->parent = node->current;
            newNode->current = fileHandler->writeIndex;
            newNode->size = N - 1;
            for (i = 0; i < (N - 1); i++)
            {
                newNode->keys[i] = childNode->keys[i];
                newNode->children[i] = childNode->children[i];
                newNode->data[i] = childNode->data[i];
            }
            newNode->children[i] = childNode->children[i];
            recordType* movedDataUp = &childNode->data[i];
            insertRecord(node, movedDataUp, newNode->current);
            if (!newNode->isLeaf)
            {
                for (int j = 0; j <= newNode->size; j++)
                {
                    Node<recordType, keyType>* updateChildNode = fileHandler->readNode(newNode->children[i]);
                    updateChildNode->parent = newNode->current;
                    fileHandler->writeNode(updateChildNode->current, updateChildNode);
                    delete updateChildNode;
                }
            }
            fileHandler->writeNode(newNode->current, newNode);
            delete newNode;
            int j = 0;
            for (i++; j < (N - 1); i++, j++)
            {
                _keys[j] = childNode->keys[i];
                _children[j] = childNode->children[i];
                _data[j] = childNode->data[i];
            }
            _children[j] = childNode->children[i];
            delete[] childNode->keys;
            delete[] childNode->children;
            delete[] childNode->data;
            childNode->keys = _keys;
            childNode->children = _children;
            childNode->data = _data;
            childNode->size = N - 1;
            fileHandler->writeNode(childNode->current, childNode);
        }
        delete childNode;
    }
    else
    {
        insertRecord(node, record);
    }
    fileHandler->writeNode(node->current, node);
    return node;
}

template<typename recordType, typename keyType>
void BTree<recordType, keyType>::insertRecord(Node<recordType, keyType> *node, recordType *record, indexType child) {
    int i;
    auto *_keys = new keyType[2 * N - 1];
    auto *_children = new indexType[2 * N];
    auto *_data = new recordType[2 * N - 1]{};
    for (i = 0; i < (2 * N - 1); i++)
        _keys[i] = 0;
    for (i = 0; i < (2 * N); i++)
        _children[i] = 0;
    for (i = 0; i < node->size; i++)
    {
        if (record->telephone > node->keys[i])
        {
            _keys[i] = node->keys[i];
            _children[i] = node->children[i];
            _data[i] = node->data[i];
        }
        else
            break;
    }
    _keys[i] = record->telephone;
    _children[i] = child;
    _data[i] = *record;
    node->size++;
    for (i++; i < node->size; i++)
    {
        _keys[i] = node->keys[i-1];
        _data[i] = node->data[i-1];
        _children[i] = node->children[i-1];
    }
    _children[i] = node->children[i-1];
    delete[] node->keys;
    delete[] node->children;
    delete[] node->data;
    node->keys = _keys;
    node->children = _children;
    node->data = _data;
}

template<typename recordType, typename keyType>
recordType *BTree<recordType, keyType>::findRecord(keyType key, bool withPrint) {
    recordType* record = nullptr;
    if (!fileHandler->rootIndex)
    {
        if (withPrint)
            std::cout << "________EMPTY_TREE________\n";
    }
    else
    {
        int count = 0;
        if (withPrint)
            std::cout << "_________FIND_HISTORY______\n";
        record = findRecordInChildTree(key, fileHandler->rootIndex, count, withPrint);
        if (withPrint)
            std::cout << "___________________________\n";
    }
    return record;
}

template<typename recordType, typename keyType>
recordType *BTree<recordType, keyType>::findRecordInChildTree(keyType key, indexType index, int count, bool withPrint) {
    if (!index)
        return nullptr;
    else
    if (withPrint)
        printHandler->printTab(count);
    recordType* record = nullptr;
    Node<recordType, keyType>* node = fileHandler->readNode(index);
    if (withPrint)
        printHandler->printNode(node);
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
            record = &(node->data[i]);
            break;
        }
    if (i == node->size)
        record = findRecordInChildTree(key, node->children[node->size], count, withPrint);
    delete node;
    return record;
}

template<typename recordType, typename keyType>
Node<recordType, keyType> *BTree<recordType, keyType>::findNodeInChildTree(keyType key, indexType index) {
    if (!index)
        return nullptr;
    Node<recordType, keyType>* node = fileHandler->readNode(index);
    indexType next;
    for (int i = 0; i < node->size; i++)
        if (key < node->keys[i])
        {
            next = node->children[i];
            delete node;
            return findNodeInChildTree(key, next);
        }
        else if (key == node->keys[i])
            return node;
    next = node->children[node->size];
    delete node;
    return findNodeInChildTree(key, next);
}

template<typename recordType, typename keyType>
unsigned BTree<recordType, keyType>::getRandomInt(unsigned int minValue, unsigned int maxValue) {
    if (minValue < maxValue)
    {
        std::uniform_int_distribution<unsigned> distribution(minValue, maxValue);
        return distribution(generator.engine);
    }
    else
    {
        std::cout << "Error random INT: minValue > maxValue !!!\n";
        exit(1);
    }
}

template<typename recordType, typename keyType>
unsigned long long BTree<recordType, keyType>::getRandomULL(unsigned long long int minValue, unsigned long long int maxValue) {
    if (minValue < maxValue)
    {
        std::uniform_int_distribution<unsigned long long> distribution(minValue, maxValue);
        return distribution(generator.engine);
    }
    else
    {
        std::cout << "Error random INT: minValue > maxValue !!!\n";
        exit(1);
    }
}

template<typename recordType, typename keyType>
void BTree<recordType, keyType>::initGenerator() {
    std::random_device device;
    generator.engine.seed(device());
}

template<typename recordType, typename keyType>
BTree<recordType, keyType>::~BTree() {}

template <typename recordType, typename keyType>
BTree<recordType, keyType>::BTree()
{
    fileHandler = new FileHandler<recordType, keyType>();
    N = fileHandler->N;
    printHandler = new PrintHandler<recordType, keyType>(*fileHandler);
    initGenerator();
}