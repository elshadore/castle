#ifndef CHAIN_TABLE_HEADER_GUARD__
#define CHAIN_TABLE_HEADER_GUARD__

#include "shared.h"

typedef struct Slot_ Slot;

typedef struct ChainTable_ {
    Slot **e;
    size_t count;
    size_t capacity;
} ChainTable;

#endif

struct Slot_ {
    String key;
    int value;
    Slot *next;
};

ChainTable chain_init(size_t capacity);
void chain_deinit(ChainTable *table);
bool chain_put(ChainTable *table, String key, int value);
bool chain_lookup(ChainTable *table, String key, int *output);
bool chain_remove(ChainTable *table, String key);
void chain_debug(ChainTable *table);
void chain_testicle(void);

#ifdef CHAIN_TABLE_IMPLEMENTATION

ChainTable chain_init(size_t capacity) {
    Slot **e = malloc(sizeof(Slot) * capacity);
    for (size_t i = 0; i < capacity; ++i) {
        e[i] = NULL;
    }
    return (ChainTable) {
        .e = e,
        .count = 0,
        .capacity = capacity,
    };
}

Slot *slot_new(String key, int value) {
    Slot *slot = malloc(sizeof(Slot));
    *slot = (Slot) {
        .key = key,
        .next = NULL,
        .value = value,
    };
    return slot;
}

float chain_fillpc(ChainTable *table) {
    return compute_fillpc(table->count, table->capacity);
}

bool slot_append(Slot *slot, String key, int value) {
    if (slot->next == NULL) {
        slot->next = slot_new(key, value);
        return true;
    } else {
        if (string_eq(slot->key, key)) return false;
        return slot_append(slot->next, key, value);
    }
}

void chain_reappend_and_free(ChainTable *table, Slot *slot) {
    if(slot == NULL) return;
    chain_put(table, slot->key, slot->value);
    Slot *next = slot->next;
    free(slot);
    chain_reappend_and_free(table, next);
}

void chain_expand_and_rehash(ChainTable *table) {
    size_t old_capacity = table->capacity;
    size_t new_capacity = table->capacity * 2;
    Slot **new_e = (Slot **)malloc(sizeof(Slot *) * new_capacity);
    Slot **old_e = table->e;
    table->e = new_e;
    table->capacity = new_capacity;
    table->count = 0;
    for (size_t i = 0; i < new_capacity; ++i) {
        new_e[i] = NULL;
    }
    for (size_t i = 0; i < old_capacity; ++i) {
        chain_reappend_and_free(table, old_e[i]);
    }
}

bool chain_put(ChainTable *table, String key, int value) {
    float pc = chain_fillpc(table);
    if (pc >= 0.8f) {
        chain_expand_and_rehash(table);
    }
    size_t hash = djb2_hash(key);
    size_t index = hash % table->capacity;
    Slot *collision = table->e[index];
    if (collision == NULL) {
        table->e[index] = slot_new(key, value);
        table->count += 1;
        return true;
    } else {
        if (slot_append(collision, key, value)) {
            table->count += 1;
            return true;
        } else {
            return false;
        }
    }
}

Slot *slot_lookup(Slot *slot, String key) {
    if (slot == NULL) return NULL;
    if (string_eq(slot->key, key)) return slot;
    return slot_lookup(slot->next, key);
}

Slot *chain_lookup_slot(ChainTable *table, String key) {
    size_t hash = djb2_hash(key);
    size_t index = hash % table->capacity;
    Slot *slot = table->e[index];
    return slot_lookup(slot, key);
}

bool chain_lookup(ChainTable *table, String key, int *output) {
    Slot *slot = chain_lookup_slot(table, key);
    if (slot == NULL) return false;
    *output = slot->value;
    return true;
}

bool chain_remove_recursive(Slot *slot, String key) {
    Slot *next = slot->next;
    if (next == NULL) return false;
    if (string_eq(next->key, key)) {
        slot->next = next->next;
        return true;
    } else {
        return chain_remove_recursive(next, key);
    }
}

bool chain_remove(ChainTable *table, String key) {
    size_t hash = djb2_hash(key);
    size_t index = hash % table->capacity;
    Slot *slot = table->e[index];
    if (slot == NULL) return false;
    if (string_eq(slot->key, key)) {
        table->e[index] = slot->next;
        table->count -= 1;
        return true;
    } else {
        if (chain_remove_recursive(slot, key)) {
            table->count -= 1;
            return true;
        } else {
            return false;
        }
    }
}

void chain_debug_slot(Slot *slot) {
    if (slot == NULL) return;
    size_t hash = djb2_hash(slot->key);
    printf("(%zu, %s, %i) ", hash, slot->key.c, slot->value);
    chain_debug_slot(slot->next);
}

void chain_debug(ChainTable *table) {
    float fillpc = chain_fillpc(table);
    printf("Table Debug %zu, %zu, (%f, %zu%%)\n", table->count, table->capacity, fillpc, (size_t)(fillpc * 100.0f));
    for (size_t i = 0; i < table->capacity; ++i) {
        printf("[%zu] => ", i);
        Slot *slot = table->e[i];
        chain_debug_slot(slot);
        printf("\n");
    }
}

void chain_deinit(ChainTable *table) {
    if (table->capacity > 0) {
        free(table->e);
    }
    *table = (ChainTable){0};
}

void chain_testicle(void) {
    ChainTable t = chain_init(8);
    assert(chain_put(&t, strlit("foo"), 69));
    assert(chain_put(&t, strlit("bar"), 1));
    assert(chain_put(&t, strlit("baz"), -1));
    assert(chain_put(&t, strlit("quz"), 42));
    assert(chain_put(&t, strlit("quux"), 9000));
    assert(chain_put(&t, strlit("bingo"), 21));
    assert(chain_put(&t, strlit("bongo"), 67));
    assert(chain_put(&t, strlit("alakazam"), 420));
    assert(chain_put(&t, strlit("bar"), 8008));
    assert(chain_remove(&t, strlit("bar")));
    int result = 0;
    assert(chain_lookup(&t, strlit("bongo"), &result));
    printf("result => %d\n", result);
    chain_debug(&t);
    chain_deinit(&t);
}

#endif
