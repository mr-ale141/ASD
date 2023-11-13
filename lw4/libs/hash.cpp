#include "headers/hash.hpp"

HashString::HashString()
{
    unsigned long long pAlpha_pow = pAlpha;
    pAlpha_pows.push_back(1);
    pAlpha_pows.push_back(pAlpha);
    int lenBitPAlpha = getBitLen(pAlpha);
    while (getBitLen(pAlpha_pow *= pAlpha) - getBitLen(*(pAlpha_pows.end() - 1)) >= lenBitPAlpha - 1)
        pAlpha_pows.push_back(pAlpha_pow);

    int lenBitPByte = getBitLen(pByte);
    unsigned long long pByte_pow = pByte;
    pByte_pows.push_back(1);
    pByte_pows.push_back(pByte);
    while (getBitLen(pByte_pow *= pByte) - getBitLen(*(pByte_pows.end() - 1)) >= lenBitPByte - 1)
        pByte_pows.push_back(pByte_pow);
}

int HashString::getBitLen(unsigned long long num)
{
    int count = 0;
    while (num)
    {
        num = num >> 1;
        count++;
    }
    return count;
}

unsigned long long HashString::getHashStringAlpha(std::string str)
{
    unsigned long long hash = 0;
    unsigned long long last_pAlpha_pow = *(pAlpha_pows.end() - 1);
    for (size_t i = 0; i < str.length(); i++)
        if (i <= pAlpha_pows.size())
        {
            if (std::isupper(str[i]))
                hash += (str[i] - 'A' + 1) * pAlpha_pows[i];
            else
                hash += (str[i] - 'a' + 1) * pAlpha_pows[i];
        }
        else
        {
            if (std::isupper(str[i]))
            {
                last_pAlpha_pow *= pAlpha;
                hash += (str[i] - 'A' + 1) * last_pAlpha_pow;
            }
            else
            {
                last_pAlpha_pow *= pAlpha;
                hash += (str[i] - 'a' + 1) * last_pAlpha_pow;
            }
        }
    return hash;
}

unsigned long long HashString::getHashStringByte(std::string str)
{
    unsigned long long hash = 0;
    unsigned long long last_pByte_pow = *(pByte_pows.end() - 1);
    for (size_t i = 0; i < str.length(); i++)
        hash += str[i] * pByte_pows[i];
    return hash;
}

void HashString::printPows()
{
    for (int i = 0; i < pAlpha_pows.size(); i++)
        printf("%2d : %llu\n", i, pAlpha_pows[i]);

    for (int i = 0; i < pByte_pows.size(); i++)
        printf("%2d : %llu\n", i, pByte_pows[i]);
}