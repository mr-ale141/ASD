#pragma once

typedef unsigned long long keyType;

struct recordPhone {
    char firstName[32] = {0};
    char secondName[32] = {0};
    int age = 0;
    unsigned long long telephone = 0ULL;
};

struct recordBirth {
    char name[32] = {0};
    int birthYear = 0;
    unsigned long long telephone = 0ULL;
};


class Record {
public:
    explicit Record(keyType newKey) : key(newKey) {}
    explicit Record() : key(0ULL) {}
    keyType key;
};

class RecordPhone : public Record {
public:
    explicit RecordPhone(recordPhone& newRecord)
    : record(newRecord), Record(newRecord.telephone) {}
    explicit RecordPhone()
            : record(*(new recordPhone)) {}
    recordPhone record;
};

class RecordBirth : public Record {
public:
    explicit RecordBirth(recordBirth& newRecord)
    : record(newRecord), Record(newRecord.telephone) {}
    explicit RecordBirth()
            : record(*(new recordBirth)) {}
    recordBirth record;
};