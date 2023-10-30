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

static const size_t bucketCounts[] = {509, 1021, 2039, 4093, 8191, 
16381, 32749, 65521};

/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
   inclusive. */
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
    SymTable_T oSymTable = malloc(sizeof(struct SymTable));
    assert(oSymTable != NULL);

    oSymTable->length = 0;
    oSymTable->bucket_ct_i = 0;
    oSymTable->buckets = calloc(bucketCounts[0], sizeof(Binding_T));
    assert(oSymTable->buckets != NULL);

    return oSymTable;
}

void SymTable_free(SymTable_T oSymTable) {
    assert(oSymTable != NULL);
    size_t i;
    for (i = 0; i < bucketCounts[oSymTable->bucket_ct_i]; ++i) {
        Binding_T current = oSymTable->buckets[i];
        while (current != NULL) {
            Binding_T next = current->next;
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
};

int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void 
*pvValue) {
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    if (SymTable_contains(oSymTable, pcKey)) {
        return 0;
    }

    size_t index = SymTable_hash(pcKey, bucketCounts[oSymTable->
    bucket_ct_i]);
    Binding_T newBinding = malloc(sizeof(struct Binding));
    assert(newBinding != NULL);

    newBinding->uKey = strdup(pcKey);
    newBinding->uValue = (void *)pvValue;
    newBinding->next = oSymTable->buckets[index];
    oSymTable->buckets[index] = newBinding;
    oSymTable->length++;

    return 1;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const 
void *pvValue) {
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    size_t index = SymTable_hash(pcKey, bucketCounts[oSymTable->
    bucket_ct_i]);
    Binding_T current = oSymTable->buckets[index];
    while (current != NULL) {
        if (strcmp(current->uKey, pcKey) == 0) {
            void *oldValue = current->uValue;
            current->uValue = (void *)pvValue;
            return oldValue;
        }
        current = current->next;
    }
    return NULL;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    size_t index = SymTable_hash(pcKey, bucketCounts[oSymTable->
    bucket_ct_i]);
    Binding_T current = oSymTable->buckets[index];

    while (current) {
        if (strcmp(current->uKey, pcKey) == 0) {
            return 1;  
        }
        current = current->next;
    }
    return 0;  
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    size_t index = SymTable_hash(pcKey, bucketCounts[oSymTable->
    bucket_ct_i]);
    Binding_T current = oSymTable->buckets[index];
    while (current != NULL) {
        if (strcmp(current->uKey, pcKey) == 0) {
            return current->uValue;
        }
        current = current->next;
    }
    return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    size_t index = SymTable_hash(pcKey, bucketCounts[oSymTable->
    bucket_ct_i]);
    Binding_T current = oSymTable->buckets[index];
    Binding_T previous = NULL;

    while (current != NULL) {
        if (strcmp(current->uKey, pcKey) == 0) {
            if (previous == NULL) {
                oSymTable->buckets[index] = current->next;
            } else {
                previous->next = current->next;
            }
            void *value = current->uValue;
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

void SymTable_map(SymTable_T oSymTable, void (*pfApply)(const char 
*pcKey, void *pvValue, void *pvExtra), const void *pvExtra) {
    assert(oSymTable != NULL);
    assert(pfApply != NULL);

    size_t i;
    for (i = 0; i < bucketCounts[oSymTable->bucket_ct_i]; ++i) {
        Binding_T current = oSymTable->buckets[i];
        while (current != NULL) {
            pfApply(current->uKey, current->uValue, (void *)pvExtra);
            current = current->next;
        }
    }
}