#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdlib.h>
/* SymTable_T is a pointer to a SymTable object. */
typedef struct SymTable *SymTable_T;

/* SymTable_new() creates a new empty SymTable object and returns a pointer to it.
 * Returns NULL if memory allocation failed. */
SymTable_T SymTable_new(void);

/* SymTable_free() frees all memory allocated for oSymTable.
 * Does nothing if oSymTable is NULL. */
void SymTable_free(SymTable_T oSymTable);

/* SymTable_getLength() returns the number of bindings in oSymTable. */
size_t SymTable_getLength(SymTable_T oSymTable);

/* SymTable_put() inserts a new binding in oSymTable.
 * Returns 1 if a new binding was inserted, 0 if a binding with the same key
 * already existed, and -1 if memory allocation failed. */
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

#endif