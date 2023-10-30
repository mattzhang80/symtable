#include "symtable.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct Binding {
    char *uKey;
    void *uValue;
    struct Binding *next;
} *Binding_T;

struct SymTable {
    size_t length;
    size_t bucket_ct_i;
    Binding_T *buckets;
};

static const size_t auBucketCounts[] = {509, 1021, 2039, 4093, 8191, 16381, 32749, 65521};

static size_t SymTable_hash(const char *pcKey, size_t uBucketCount) {
    const size_t HASH_MULTIPLIER = 65599;
    size_t u;
    size_t uHash = 0;

    assert(pcKey != NULL);

    for (u = 0; pcKey[u] != '\0'; u++)
        uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

    return uHash % uBucketCount;
}

SymTable_T SymTable_new(void) {
    SymTable_T oSymTable;
    oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
    if (oSymTable == NULL) {
        return NULL;
    }

    oSymTable->length = 0;
    oSymTable->bucket_ct_i = 0;
    oSymTable->buckets = (Binding_T *)calloc(auBucketCounts[0], sizeof(Binding_T));
    if (oSymTable->buckets == NULL) {
        free(oSymTable);
        return NULL;
    }

    return oSymTable;
}

void SymTable_free(SymTable_T oSymTable) {
    size_t i;
    Binding_T current, next;

    if (oSymTable == NULL) {
        return;
    }

    for (i = 0; i < auBucketCounts[oSymTable->bucket_ct_i]; ++i) {
        current = oSymTable->buckets[i];
        while (current != NULL) {
            next = current->next;
            free(current->uKey);
            free(current);
            current = next;
        }
    }
    free(oSymTable->buckets);
    free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable) {
    assert(oSymTable != NULL);
    return oSymTable->length;
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue) {
    Binding_T newBinding;
    size_t index;
    char *keyCopy;

    if (oSymTable == NULL || pcKey == NULL) {
        return 0;
    }

    if (SymTable_contains(oSymTable, pcKey)) {
        return 0;
    }

    index = SymTable_hash(pcKey, auBucketCounts[oSymTable->bucket_ct_i]);
    keyCopy = (char *)malloc(strlen(pcKey) + 1);
    if (keyCopy == NULL) return 0;
    strcpy(keyCopy, pcKey);

    newBinding = (Binding_T)malloc(sizeof(struct Binding));
    if (newBinding == NULL) {
        free(keyCopy);
        return 0;
    }

    newBinding->uKey = keyCopy;
    newBinding->uValue = (void *)pvValue;
    newBinding->next = oSymTable->buckets[index];
    oSymTable->buckets[index] = newBinding;
    oSymTable->length++;

    return 1;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue) {
    size_t index;
    Binding_T current;
    void *oldValue;

    if (oSymTable == NULL || pcKey == NULL) {
        return NULL;
    }

    index = SymTable_hash(pcKey, auBucketCounts[oSymTable->bucket_ct_i]);
    current = oSymTable->buckets[index];

    while (current != NULL) {
        if (strcmp(current->uKey, pcKey) == 0) {
            oldValue = current->uValue;
            current->uValue = (void *)pvValue;
            return oldValue;
        }
        current = current->next;
    }
    return NULL;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    size_t index;
    Binding_T current;

    if (oSymTable == NULL || pcKey == NULL) {
        return 0;
    }

    index = SymTable_hash(pcKey, auBucketCounts[oSymTable->bucket_ct_i]);
    current = oSymTable->buckets[index];

    while (current) {
        if (strcmp(current->uKey, pcKey) == 0) {
            return 1;  
        }
        current = current->next;
    }
    return 0;  
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
    size_t index;
    Binding_T current;

    if (oSymTable == NULL || pcKey == NULL) {
        return NULL;
    }

    index = SymTable_hash(pcKey, auBucketCounts[oSymTable->bucket_ct_i]);
    current = oSymTable->buckets[index];

    while (current != NULL) {
        if (strcmp(current->uKey, pcKey) == 0) {
            return current->uValue;
        }
        current = current->next;
    }
    return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
    size_t index;
    Binding_T current, previous;
    void *value;

    if (oSymTable == NULL || pcKey == NULL) {
        return NULL;
    }

    index = SymTable_hash(pcKey, auBucketCounts[oSymTable->bucket_ct_i]);
    current = oSymTable->buckets[index];
    previous = NULL;

    while (current != NULL) {
        if (strcmp(current->uKey, pcKey) == 0) {
            if (previous == NULL) {
                oSymTable->buckets[index] = current->next;
            } else {
                previous->next = current->next;
            }
            value = current->uValue;
            free(current->uKey);
            free(current);
            oSymTable->length--;
            return value;
        }
        previous = current;
        current = current->next;
    }
    return NULL;
}

void SymTable_map(SymTable_T oSymTable, void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), const void *pvExtra) {
    size_t i;
    Binding_T current;

    if (oSymTable == NULL || pfApply == NULL) {
        return;
    }

    for (i = 0; i < auBucketCounts[oSymTable->bucket_ct_i]; ++i) {
        current = oSymTable->buckets[i];
        while (current != NULL) {
            pfApply(current->uKey, current->uValue, (void *)pvExtra);
            current = current->next;
        }
    }
}