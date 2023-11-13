#include "headers/b_tree.hpp"

BTree::BTree(int N, char* file_name, recordStruct &firstData)
{
    BTreeRoot.N = N;
    file.open(file_name, std::fstream::in | std::fstream::out | std::fstream::binary);
    if (!file)
    {
        file.open(file_name, std::fstream::binary | std::fstream::trunc | std::fstream::out);
        file.close();
        file.open(file_name, std::fstream::binary | std::fstream::in | std::fstream::out);
        BTreeRoot.root.isLeaf = true;
        BTreeRoot.root.keys[0] = firstData.telephone;
        BTreeRoot.root.keys[1] = 0;
        BTreeRoot.root.keys[2] = 0;
        BTreeRoot.root.childs[0] = 1;
        BTreeRoot.root.childs[1] = 0;
        BTreeRoot.root.childs[2] = 0;
        file.write((char *)&BTreeRoot.root, sizeof(BTreeRoot.root));
        file.write((char *)&firstData, sizeof(firstData));
    }
    else
    {
        file.read((char *)&BTreeRoot.root, sizeof(BTreeRoot.root));
    }
    file.close();
}