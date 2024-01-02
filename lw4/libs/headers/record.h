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
    recordPhone record;

    explicit RecordPhone(recordPhone& newRecord)
    : Record(newRecord.telephone), record(newRecord) {}

    explicit RecordPhone() : Record()
    {
        recordPhone zeroRecord;
        record = zeroRecord;
    }
};

class RecordBirth : public Record {
public:
    recordBirth record;

    explicit RecordBirth(recordBirth& newRecord)
    : Record(newRecord.telephone), record(newRecord) {}

    explicit RecordBirth() : Record()
    {
        recordBirth zeroRecord;
        record = zeroRecord;
    }
};