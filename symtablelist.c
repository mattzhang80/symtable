/*symtablelist implements a Symbol Table ADT using a linked list, with
functions to create a new symbol table, free a symbol table, get the 
length of a symbol table, put a key-value pair into the symbol table, 
replace the value of a key-value with another value, 
check if a symbol table contains a key, gets the value from a key-value, 
removes a key-value, and applies a function to all key-value Bindings*/
#include "symtable.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* Binding is a node in the linked list that represents a symbol table. 
   It contains a key-value pair and a pointer to the next node. */
struct Binding {
    char *uKey;
    void *uValue;
    struct Binding *next;
};

/* SymTable is a linked list of Bindings. It contains a pointer to the 
   first node in the list, and the length of the list. */
struct SymTable {
    struct Binding *head;
    size_t length;
};

/* SymTable_new: Create and initialize a new symbol table. 
Return NULL if memory allocation fails. */
SymTable_T SymTable_new(void) {
    SymTable_T oSymTable = malloc(sizeof(struct SymTable));
    if (!oSymTable) {
        return NULL;  
    }
    oSymTable->head = NULL;
    oSymTable->length = 0;
    return oSymTable;
}

/* SymTable_new: Allocates and returns a new symbol table, initializing 
it to be empty. Returns NULL if memory allocation fails. 
The client is responsible for eventually freeing the symbol table with 
SymTable_free. */
void SymTable_free(SymTable_T oSymTable) {
    struct Binding *current = oSymTable->head;
    struct Binding *next;
    if (!oSymTable) return;    

    while (current != NULL) {
        next = current->next;
        free(current->uKey);
        free(current);
        current = next;
    }
    free(oSymTable);
}

/* SymTable_getLength: Returns the number of bindings in the symbol 
table. The client must pass a valid symbol table pointer. */
size_t SymTable_getLength(SymTable_T oSymTable) {
    assert(oSymTable != NULL);
    return oSymTable->length;
}

/* SymTable_put: Adds a new binding with the specified key and value to 
the symbol table. Returns 1 if successful, or 0 if the key already 
exists or memory allocation fails. The client must pass a valid symbol 
table pointer and a non-NULL key. The key is copied. The copy is 
owned by the symbol table. */
int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
    const void *pvValue) {
        struct Binding *newB;
        char *keyCopy;
        size_t keyLength;

        assert(oSymTable != NULL);
        assert(pcKey != NULL);

        if (SymTable_contains(oSymTable, pcKey)) {
            return 0;
        }

        keyLength = strlen(pcKey) + 1;
        keyCopy = (char *)malloc(keyLength);
        if (keyCopy == NULL) {
            return 0; 
        }   
        strcpy(keyCopy, pcKey);

       newB = (struct Binding *)malloc(sizeof(struct Binding));
        if (newB == NULL) {
            free(keyCopy);  
            return 0; 
        }

        newB->uKey = keyCopy;
        newB->uValue = (void *)pvValue;
        newB->next = oSymTable->head;
        oSymTable->head = newB;
        oSymTable->length++;

        return 1;
}

/* SymTable_replace: If a binding with the specified key exists, 
replaces its value and returns the old value. Otherwise, returns NULL. 
The client must pass a valid symbol table pointer and a non-NULL key. */
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const 
    void *pvValue) {
    struct Binding *curr = oSymTable->head;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    while (curr != NULL) {
        if (strcmp(curr->uKey, pcKey) == 0) {
            void *prevValue = curr->uValue;
            curr->uValue = (void *)pvValue;
            return prevValue;
        }
        curr = curr->next;
    }
    return NULL;
}

/* SymTable_contains: Checks if a binding with the specified key exists 
in the symbol table. Returns 1 if it exists, or 0 otherwise. 
The client must pass a valid symbol table pointer and a non-NULL key. */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    struct Binding *curr = oSymTable->head;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    while (curr != NULL) {
        if (strcmp(curr->uKey, pcKey) == 0) {
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}

/* SymTable_get: If a binding with the specified key exists, returns its 
value. Otherwise, returns NULL. The client must pass a valid symbol 
table pointer and a non-NULL key. */

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
    struct Binding *curr = oSymTable->head;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    while (curr != NULL) {
        if (strcmp(curr->uKey, pcKey) == 0) {
            return curr->uValue;
        }
        curr = curr->next;
    }
    return NULL;
}

/* SymTable_remove: If a binding with the specified key exists, removes 
it from the symbol table, frees the memory associated with the key, and 
returns the value. Otherwise, returns NULL. The client must pass a valid 
symbol table pointer and a non-NULL key. */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
    struct Binding *curr = oSymTable->head;
    struct Binding *prev = NULL;
    void *value;

    assert(oSymTable != NULL && pcKey != NULL);

    while (curr != NULL) {
        if (strcmp(curr->uKey, pcKey) == 0) {
            if (prev == NULL) {
                oSymTable->head = curr->next;
            } else {
                prev->next = curr->next;
            }
            value = curr->uValue;
            free(curr->uKey);
            free(curr);
            oSymTable->length--;
            return value;
        }
        prev = curr;
        curr = curr->next;
    }
    return NULL;
}

/* SymTable_map: Applies the specified function to each binding in the 
symbol table. The client must pass a valid symbol table pointer and a 
non-NULL function pointer. The pvExtra parameter is passed through to 
the function. */
void SymTable_map(SymTable_T oSymTable, void (*pfApply)(const char 
*pcKey, void *pvValue, void *pvExtra), const void *pvExtra) {
    struct Binding *curr = oSymTable->head;
    assert(oSymTable != NULL);
    assert(pfApply != NULL);

    while (curr != NULL) {
        pfApply(curr->uKey, curr->uValue, (void *)pvExtra);
        curr = curr->next;
    }
}