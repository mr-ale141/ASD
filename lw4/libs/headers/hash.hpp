#pragma once
#include <iostream>
#include <string>
#include <vector>

class HashString
{
    const int pAlpha = 2;
    const int pByte = 257;
public:
    HashString();
    unsigned long long getHashAlphaString(std::string str);
    unsigned long long getHashString(std::string str);
    void printPows();
    int getBitLen(unsigned long long num);

private:
    std::vector<unsigned long long> pAlpha_pows;
    std::vector<unsigned long long> pByte_pows;
};
