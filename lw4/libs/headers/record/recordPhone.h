#pragma once
#include "record.h"

class RecordPhone : public Record {
public:
    char firstName[32] = {0};
    char secondName[32] = {0};
    int age = 0;
    unsigned long long telephone = 0ULL;
};