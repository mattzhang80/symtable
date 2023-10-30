#include "symtable.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct Binding {
    char *uKey;
    void *uValue;
    struct Binding *next;
};

struct SymTable {
    struct Binding **buckets;
    size_t length;
    size_t bucket_ct;
    size_t binding_ct;
};


/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
   inclusive. */

static size_t SymTable_hash(const char *pcKey, size_t uBucketCount)
{
   const size_t HASH_MULTIPLIER = 65599;
   size_t u;
   size_t uHash = 0;

   assert(pcKey != NULL);

   for (u = 0; pcKey[u] != '\0'; u++)
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

   return uHash % uBucketCount;
}

SymTable_T SymTable_new(void) {
    SymTable_T oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
    assert(oSymTable != NULL);

    oSymTable -> bucket_ct = (Binding **)calloc(509, sizeof(Binding *));

void SymTable_free(SymTable_T oSymTable);

size_t SymTable_getLength(SymTable_T oSymTable) {
    assert(oSymTable != NULL);
    return oSymTable -> length;
};

int SymTable_put(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue);

void *SymTable_replace(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue);

int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra);

