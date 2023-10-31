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
    /* The key is a string. */
    char *uKey;
    /* The value is a void pointer. */
    void *uValue;
    /* The next pointer points to the next node in the linked list. */
    struct Binding *next;
};

/* SymTable is a linked list of Bindings. It contains a pointer to the 
   first node in the list, and the length of the list. */
struct SymTable {
    /* The head pointer points to the first node in the linked list. */
    struct Binding *head;
    /* The length of the linked list. */
    size_t length;
};

/* SymTable_new: Create and initialize a new symbol table. 
Return NULL if memory allocation fails. */
SymTable_T SymTable_new(void) {
    /* Allocate memory for the symbol table. */
    SymTable_T oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
    /* If memory allocation fails, return NULL. */
    if (!oSymTable) {
        return NULL;  
    }
    /* Initialize the symbol table. */
    oSymTable->head = NULL;
    oSymTable->length = 0;
    /* Return the symbol table. */
    return oSymTable;
}

/* SymTable_free: Frees all memory associated with the symbol table, 
including all bindings, their keys, and the table itself. The client 
must not access the symbol table after this function is called. */
void SymTable_free(SymTable_T oSymTable) {
    struct Binding *current = oSymTable->head;
    struct Binding *next;
    /* If the symbol table is NULL, return. */
    if (!oSymTable) return;    
    /* Free all the bindings in the symbol table. */
    while (current != NULL) {
        /* Store the next binding. */
        next = current->next;
        free(current->uKey);
        free(current);
        current = next;
    }
    /* Free the symbol table. */
    free(oSymTable);
}

/* SymTable_getLength: Returns the number of bindings in the symbol 
table. The client must pass a valid symbol table pointer. */
size_t SymTable_getLength(SymTable_T oSymTable) {
    /* Check for NULL symbol table. */
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
        /* If the symbol table or key or value is NULL, return. */
        assert(oSymTable != NULL);
        assert(pcKey != NULL);
        /* If the key already exists, return. */
        if (SymTable_contains(oSymTable, pcKey)) {
            return 0;
        }
        /* Allocate memory for the key. */
        keyLength = strlen(pcKey) + 1;
        keyCopy = (char *)malloc(keyLength);
        /* If memory allocation fails, return. */
        if (keyCopy == NULL) {
            return 0; 
        }   
        strcpy(keyCopy, pcKey);
        /* Allocate memory for the binding. */
       newB = (struct Binding *)malloc(sizeof(struct Binding));
       /* If memory allocation fails, free the key and return. */
        if (newB == NULL) {
            free(keyCopy);  
            return 0; 
        }
        /* Initialize the binding. */
        newB->uKey = keyCopy;
        newB->uValue = (void *)pvValue;
        newB->next = oSymTable->head;
        oSymTable->head = newB;
        oSymTable->length++;
        /* Return 1 to indicate success. */
        return 1;
}

/* SymTable_replace: If a binding with the specified key exists, 
replaces its value and returns the old value. Otherwise, returns NULL. 
The client must pass a valid symbol table pointer and a non-NULL key. */
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue) {
    /* Creates a current pointer to the head of the symbol table. */
    struct Binding *current = oSymTable->head;
    /* If the symbol table or key or value is NULL, return. */
    assert(oSymTable != NULL && pcKey != NULL && pvValue != NULL);
    /* Iterates through the symbol table until the key is found. */
    while (current != NULL) {
        /* If the key is found, replace the value and return the old 
        value. */
        if (strcmp(current->uKey, pcKey) == 0) {
            void *oldValue = current->uValue;
            current->uValue = (void *)pvValue;
            return oldValue;
        }
        /* Otherwise, continue iterating through the symbol table. */
        current = current->next;
    }
    /* If the key is not found, return NULL. */
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
    assert(pvExtra != NULL);

    while (curr != NULL) {
        pfApply(curr->uKey, curr->uValue, (void *)pvExtra);
        curr = curr->next;
    }
}