#pragma once

typedef struct recordStruct
{
    char firstName[32] = {0};
    char secondName[32] = {0};
    int age = 0;
    unsigned long long telephone = 0ULL;
} recordType;

typedef struct recordStruct2
{
    char name[32] = {0};
    int birthYear = 0;
    unsigned long long telephone = 0ULL;
} recordType2;