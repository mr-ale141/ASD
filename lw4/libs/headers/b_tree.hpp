#pragma once

class BTree
{
public:
    BTree(int N, char* file_name);
    int ins(char* str);
    void del(char *str);
    char* find(char *str);

private:

};
