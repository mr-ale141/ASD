#pragma once
#include "record.h"

class RecordBirth : public Record {
public:
    char name[32] = {0};
    int birthYear = 0;
    unsigned long long telephone = 0ULL;
};