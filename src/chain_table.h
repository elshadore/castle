#ifndef CHAIN_TABLE_H
#define CHAIN_TABLE_H

#include "shared.h"

typedef struct Slot_ Slot;

typedef struct ChainTable_ {
    Slot **e;
    size_t count;
    size_t capacity;
} ChainTable;

#endif

