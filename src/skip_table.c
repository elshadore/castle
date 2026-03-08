#include "skip_table.h"

SkipTable skip_new(void) {
    return (SkipTable){0};
}

void skip_expand(SkipTable *t);

SkipSlot *skip_lookup(SkipTable *t, String key) {
    if (t->capacity == 0) {
        return NULL;
    }
    size_t hash = djb2_hash(key);
    size_t index = hash % t->capacity;
    for (;;) {
        SkipSlot *slot = &t->e[index];
        if (slot->state == SkipSlotState_Free) {
            return NULL;
        }
        if (slot->state == SkipSlotState_Occupied) {
            if (string_eq(key, slot->key)) {
                return slot;
            }
        }
        // linear probing
        index = (index + 1) % t->capacity;
    }
}

void skip_insert(SkipTable *t, String key, int value) {
    SkipSlot *slot = skip_lookup(t, key);
    if (slot != NULL) {
        slot->value = value;
        return;
    }
    slot = NULL;
    skip_expand(t);
    size_t hash = djb2_hash(key);
    size_t index = hash % t->capacity;
    for (;;) {
        SkipSlot *slot = &t->e[index];
        if (slot->state != SkipSlotState_Occupied) {
            *slot = (SkipSlot){
                .key = key,
                .state = SkipSlotState_Occupied,
                .value = value,
            };
            t->count += 1;
            return;
        }
        // linear probing
        // dlog("collison at index => %zu, occupied => %s, inserting => %s", index, slot->key.c, key.c);
        index = (index + 1) % t->capacity;
    }
}

bool skip_remove(SkipTable *t, String key) {
    SkipSlot *slot = skip_lookup(t, key);
    if (slot == NULL) {
        return true;
    }
    *slot = (SkipSlot) {0};
    slot->state = SkipSlotState_Deleted;
    t->count -= 1;
    return false;
}

void skip_expand(SkipTable *t) {
    if (t->count < t->capacity) {
        float fill_pc = compute_fillpc(t->count, t->capacity);
        if (fill_pc < 0.75f) {
            return;
        }
    }
    size_t old_capacity = t->capacity;
    size_t new_capacity = grow_capacity(old_capacity);

    dlog("skip expand => %zu = %zu", old_capacity, new_capacity);

    size_t size = sizeof(SkipSlot) * new_capacity;
    SkipSlot *new_e = (SkipSlot *)malloc(size);
    memset(new_e, 0, size);
    SkipSlot *old_e = t->e;
    t->capacity = new_capacity;
    t->count = 0;
    t->e = new_e;
    // rehashing after expand
    for (size_t i = 0; i < old_capacity; ++i) {
        SkipSlot slot = old_e[i];
        if (slot.state == SkipSlotState_Occupied) {
            skip_insert(t, slot.key, slot.value);
        }
    }
    if (old_e != NULL) {
        free(old_e);
    }
}

void skip_debug(SkipTable *t) {
    float fill_pc = compute_fillpc(t->count, t->capacity);
    printf("*~SKIP-DEBUG~* => (%f)\n", fill_pc);
    for (size_t i = 0; i < t->capacity; ++i) {
        printf("[%zu] => ", i);
        SkipSlot slot = t->e[i];
        switch (slot.state) {
            case SkipSlotState_Deleted: {
                printf("Deleted");
                break;
            }
            case SkipSlotState_Free: {
                printf("Free");
                break;
            }
            case SkipSlotState_Occupied: {
                printf("(%s) => %d", slot.key.c, slot.value);
                break;
            }
        }
        printf("\n");
    }
}

void skip_testicle(void) {
    dlog("floopy");
    SkipTable t = skip_new();
    skip_insert(&t, strlit("foo"), 69);
    skip_insert(&t, strlit("bar"), 420);
    skip_insert(&t, strlit("baz"), 67);
    skip_insert(&t, strlit("quuz"), 21);
    skip_insert(&t, strlit("quux"), 42);
    skip_remove(&t, strlit("bar"));
    skip_insert(&t, strlit("quuz"), 6969);
    skip_insert(&t, strlit("adam"), 1);
    skip_insert(&t, strlit("bingus"), 2);
    skip_insert(&t, strlit("fart"), 3);
    skip_debug(&t);
}
