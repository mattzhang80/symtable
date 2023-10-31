/*This header file defines the interface for a symbol table, a data 
structure that maps unique string keys to associated values. This 
implementation can create an empty symbol table, free a symbol 
table, get the length of a symbol table, put a key-value binding into 
the symbol table, replace the value of a key-value with another value, 
check if a symbol table contains a key, gets the value from a key-value, 
removes a key-value, and applies a function to all key-value Bindings*/
#ifndef SYMTABLE_H
#define SYMTABLE_H
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* SymTable_T is a pointer to a SymTable object. */
typedef struct SymTable *SymTable_T;

/* SymTable_new: Allocates, initializes, and returns a new symbol table, 
or returns NULL if memory allocation fails. The new table is initially 
empty. */
SymTable_T SymTable_new(void);

/* SymTable_free: Frees all memory associated with oSymTable, 
including all bindings, their keys, and the table itself. The client 
must not access the symbol table after this function is called. */
void SymTable_free(SymTable_T oSymTable);

/* SymTable_getLength: Returns the number of bindings in the oSymTable. 
The client must pass a valid symbol table pointer. */
size_t SymTable_getLength(SymTable_T oSymTable);

/* SymTable_put: Adds a new binding with the specified key and value to 
the oSymTable. Returns 1 if successful, or 0 if the key already 
exists, memory allocation fails, or if the symbol table or key is NULL. 
The key is copied, and the copy is owned by the symbol table. */
int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue);

/* SymTable_replace: If a binding with the specified pckey exists, 
replaces its value with pvValue and returns the old value. Otherwise, 
returns NULL. The client must pass valid symbol table & key pointers.*/
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue);

/* SymTable_contains: Checks if a binding with the specified pcKey 
exists in the oSymTable. Returns 1 if it exists, or 0 otherwise. The 
client must pass valid symbol table and key pointers. */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/* SymTable_get: If a binding with the specified pcKey exists, returns its 
value. Otherwise, returns NULL. The client must pass valid oSymTable 
and key pointers. */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/* SymTable_remove: If a binding with the specified pcKey exists, removes 
it from the oSymTable, frees the memory associated with the key, and 
returns the value. Otherwise, returns NULL. The client must pass valid 
symbol table and key pointers. */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/* SymTable_map: Applies function pfApply to each binding in the 
oSymTable, passing pvExtra as an extra parameter. The client must pass a
valid symbol table pointer and a valid function pointer. */
void SymTable_map(SymTable_T oSymTable, void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), const void *pvExtra);

#endif /* SYMTABLE_H */