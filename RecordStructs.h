#pragma once
#include <cstdint>

struct Record {
    int tconst;
    float averageRating;
    int numVotes;
};

struct RecordAddress {
    uintptr_t blockAddress;
    uint32_t offset;
};