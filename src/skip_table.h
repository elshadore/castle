#ifndef SKIP_TABLE_H
#define SKIP_TABLE_H

#include "shared.h"

typedef enum SkipSlotState_ {
    SkipSlotState_Free,
    SkipSlotState_Occupied,
    SkipSlotState_Deleted,
} SkipSlotState;

typedef struct SkipSlot_ {
    String key;
    int value;
    SkipSlotState state;
} SkipSlot;

typedef struct SkipTable_ {
    SkipSlot *e;
    size_t count;
    size_t capacity;
} SkipTable;

SkipTable skip_new(void);
void skip_insert(SkipTable *t, String key, int value);
bool skip_remove(SkipTable *t, String key);
void skip_debug(SkipTable *t);
void skip_testicle(void);
#endif

