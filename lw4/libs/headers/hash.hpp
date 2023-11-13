#pragma once
#include <iostream>
#include <string>
#include <vector>

class HashString
{
    const int pAlpha = 53;
    const int pByte = 257;
public:
    HashString();
    unsigned long long getHashStringAlpha(std::string str);
    unsigned long long getHashStringByte(std::string str);
    void printPows();
    int getBitLen(unsigned long long num);

private:
    std::vector<unsigned long long> pAlpha_pows;
    std::vector<unsigned long long> pByte_pows;
};
