#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdlib.h>

/* Opaque pointer to a symbol table. */
typedef struct SymTable *SymTable_T;

/* Function to create a new symbol table. */
SymTable_T SymTable_new(void);

/* Function to free all memory occupied by the symbol table. */
void SymTable_free(SymTable_T oSymTable);

/* Function to get the number of bindings in the symbol table. */
size_t SymTable_getLength(SymTable_T oSymTable);

/* Function to insert a new binding into the symbol table.
   Returns 1 (true) if the binding was added, and 0 (false) if the key is already in the table. */
int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue);

/* Function to replace the value of an existing binding in the symbol table.
   Returns the old value if the binding exists, and NULL otherwise. */
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue);

/* Function to check if a binding with the given key exists in the symbol table.
   Returns 1 (true) if the binding exists, and 0 (false) otherwise. */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/* Function to get the value of a binding in the symbol table.
   Returns the value if the binding exists, and NULL otherwise. */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/* Function to remove a binding from the symbol table.
   Returns the value of the removed binding if it exists, and NULL otherwise. */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/* Function to apply a function to all bindings in the symbol table. */
void SymTable_map(SymTable_T oSymTable, void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), const void *pvExtra);

#endif /* SYMTABLE_H */