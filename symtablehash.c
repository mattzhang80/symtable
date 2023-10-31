/*symtablelist implements a Symbol Table ADT using a hash table, with
functions to create an empty symbol table, free a symbol table, get the 
length of a symbol table, put a key-value pair into the symbol table, 
replace the value of a key-value with another value, 
check if a symbol table contains a key, gets the value from a key-value, 
removes a key-value, and applies a function to all key-value Bindings*/
#include "symtable.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Binding: Represents a single key-value pair in a hash table-based 
symbol table. Each binding contains a unique key, associated value, and 
a pointer to the next binding in the same bucket of the hash table. */
typedef struct Binding {
    /* Pointer to the key string. */
    char *uKey;
    /* Pointer to the value. */
    void *uValue;
    /* Pointer to the next binding in the same bucket. */
    struct Binding *next;
} 
/* Pointer to a Binding. */
*Binding_T; 

/*SymTable: A hash table-based symbol table, maps unique keys to values.
The table dynamically resizes to maintain efficient operations. */
struct SymTable {
    /* Number of bindings in the symbol table. */
    size_t length;
    /* Index of the current bucket count in the auBucketCounts array. */
    size_t bucket_ct_i;
    /* Array of pointers to the first binding in each bucket. */
    Binding_T *buckets;
};

/* auBucketCounts: An array of prime numbers representing possible 
bucket counts in the hash table, used to resize the table and maintain 
efficient operations. */
static const size_t auBucketCounts[] = {509, 1021, 2039, 4093, 8191, 
    16381, 32749, 65521};

/* SymTable_hash: Hashes a key (pcKey) to return an index for the 
buckets array, ensuring a distribution that contributes to efficient 
operations. The client must pass a non-NULL key string.  The function 
returns an index in the range [0, uBucketCount - 1], which can be used 
to access a specific bucket in the buckets array of the symbol table. */
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount) {
    const size_t HASH_MULTIPLIER = 65599;
    size_t u;
    size_t uHash = 0;
    /* Check for NULL key. */
    assert(pcKey != NULL);
    /* Hash the key. */
    for (u = 0; pcKey[u] != '\0'; u++)
        uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];
    /* Return the index in the buckets array. */
    return uHash % uBucketCount;
}

/* SymTable_new: Allocates, initializes, and returns a new symbol table, 
or returns NULL if memory allocation fails. 
The new table is initially empty and has a default number of buckets. */
SymTable_T SymTable_new(void) {
    SymTable_T oSymTable;
    /* Allocate memory for the symbol table. */
    oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
    /* Check for memory allocation failure. */
    if (oSymTable == NULL) {
        return NULL;
    }
    /* Initialize the symbol table. */
    oSymTable->length = 0;
    oSymTable->bucket_ct_i = 0;
    oSymTable->buckets = (Binding_T *)calloc(auBucketCounts[0], 
        sizeof(Binding_T));
    /* Check for memory allocation failure. */
    if (oSymTable->buckets == NULL) {
        free(oSymTable);
        return NULL;
    }
    /* Return the new symbol table. */
    return oSymTable;
}

/* SymTable_free: Frees all memory associated with the symbol table, 
including all bindings, their keys, and the table itself. The client 
must not access the symbol table after this function is called. */
void SymTable_free(SymTable_T oSymTable) {
    size_t i;
    Binding_T current, next;
    /* Check for NULL symbol table. */
    if (oSymTable == NULL) {
        return;
    }
    /* Free all bindings in the symbol table. */
    for (i = 0; i < auBucketCounts[oSymTable->bucket_ct_i]; ++i) {
        current = oSymTable->buckets[i];
        /* Free all bindings in the current bucket. */
        while (current != NULL) {
            next = current->next;
            free(current->uKey);
            free(current);
            current = next;
        }
    }
    /* Free the buckets array and the symbol table. */
    free(oSymTable->buckets);
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
exists, memory allocation fails, or if the symbol table or key is NULL. 
The key is copied, and the copy is owned by the symbol table. */
int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue) {
    Binding_T newBinding;
    size_t index;
    char *keyCopy;

    /* Check for NULL symbol table, key, or value. */
    assert(pcKey != NULL);
    if (oSymTable == NULL) {
        return 0;
    }
    /* Check if key already exists. */
    if (SymTable_contains(oSymTable, pcKey)) {
        return 0;
    }
    index = SymTable_hash(pcKey, auBucketCounts[oSymTable->bucket_ct_i]);
    keyCopy = (char *)malloc(strlen(pcKey) + 1);
    /* Check for memory allocation failure. */
    if (keyCopy == NULL) return 0;
    strcpy(keyCopy, pcKey);

    newBinding = (Binding_T)malloc(sizeof(struct Binding));
    /* Check for memory allocation failure. */
    if (newBinding == NULL) {
        free(keyCopy);
        return 0;
    }
    /* Add new binding to the symbol table. */
    newBinding->uKey = keyCopy;
    newBinding->uValue = (void *)pvValue;
    newBinding->next = oSymTable->buckets[index];
    oSymTable->buckets[index] = newBinding;
    oSymTable->length++;
    /* If the symbol table is full, resize it. */
    if (oSymTable->length == auBucketCounts[oSymTable->bucket_ct_i]) {
        /* Resize the symbol table. */
        SymTable_T newTable = SymTable_new();
        /* Check for memory allocation failure. */
        if (newTable == NULL) {
            return 0;
        }
        /* Add all bindings to the new symbol table. */
        SymTable_map(oSymTable, (void *)SymTable_put, newTable);
        /* Free the old symbol table. */
        SymTable_free(oSymTable);
        /* Update the symbol table pointer. */
        oSymTable = newTable;
    }
    /*Return 1 if successful. */
    return 1;
}
/* SymTable_replace: If a binding with the specified key exists, 
replaces its value and returns the old value. Otherwise, returns NULL. 
The client must pass valid symbol table and key pointers. */
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue) {
    size_t index;
    Binding_T current;
    void *oldValue;
    /* Check for NULL symbol table, key, or value. */
    assert(pcKey != NULL);
    if (oSymTable == NULL) {
        return NULL;
    }
    /* Check if key exists. */
    index = SymTable_hash(pcKey, auBucketCounts[oSymTable->bucket_ct_i]);
    current = oSymTable->buckets[index];
    /* Replace the value if the key exists. */
    while (current != NULL) {
        /* If the key exists, replace the value and return the old value. */
        if (strcmp(current->uKey, pcKey) == 0) {
            oldValue = current->uValue;
            current->uValue = (void *)pvValue;
            return oldValue;
        }
        current = current->next;
    }
    /* Return NULL if the key does not exist. */
    return NULL;
}

/* SymTable_contains: Checks if a binding with the specified key exists 
in the symbol table. Returns 1 if it exists, or 0 otherwise. The client 
must pass valid symbol table and key pointers. */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    size_t index;
    Binding_T current;
    assert(pcKey != NULL);
    /* Check for NULL symbol table or key. */
    if (oSymTable == NULL) {
        return 0;
    }
    /* Check if key exists. */
    index = SymTable_hash(pcKey, auBucketCounts[oSymTable->bucket_ct_i]);
    current = oSymTable->buckets[index];
    /* Return 1 if the key exists, or 0 otherwise. */
    while (current) {
        /* If the key exists, return 1. */
        if (strcmp(current->uKey, pcKey) == 0) {
            return 1;  
        }
        /* Otherwise, continue to the next binding. */
        current = current->next;
    }
    /* Return 0 if the key does not exist. */
    return 0;  
}

/* SymTable_get: If a binding with the specified key exists, returns its 
value. Otherwise, returns NULL. The client must pass valid symbol table 
and key pointers. */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
    size_t index;
    Binding_T current;
    /* Check for NULL symbol table or key. */
    assert(pcKey != NULL);
    if (oSymTable == NULL) {
        return NULL;
    }
    /* Check if key exists. */
    index = SymTable_hash(pcKey, auBucketCounts[oSymTable->bucket_ct_i]);
    current = oSymTable->buckets[index];
    /* Return the value if the key exists, or NULL otherwise. */
    while (current != NULL) {
        /* If the key exists, return the value. */
        if (strcmp(current->uKey, pcKey) == 0) {
            return current->uValue;
        }
        /* Otherwise, continue to the next binding. */
        current = current->next;
    }
    /* Return NULL if the key does not exist. */
    return NULL;
}

/* SymTable_remove: If a binding with the specified key exists, removes 
it from the symbol table, frees the memory associated with the key, and 
returns the value. Otherwise, returns NULL. The client must pass valid 
symbol table and key pointers. */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
    size_t index;
    Binding_T current, previous;
    void *value;
    /* Check for NULL symbol table or key. */
    assert(pcKey != NULL);
    if (oSymTable == NULL) {
        return NULL;
    }
    /* Check if key exists. */
    index = SymTable_hash(pcKey, auBucketCounts[oSymTable->bucket_ct_i]);
    current = oSymTable->buckets[index];
    previous = NULL;
    /* Remove the binding if the key exists. */
    while (current != NULL) {
        /* If the key exists, remove the binding and return the value. */
        if (strcmp(current->uKey, pcKey) == 0) {
            /* If the binding is the first in the bucket, update the
            bucket pointer. */
            if (previous == NULL) {
                oSymTable->buckets[index] = current->next;
            } 
            /* Otherwise, update the previous binding's next pointer. */
            else {
                previous->next = current->next;
            }
            /* Free the key and binding, and return the value. */
            value = current->uValue;
            free(current->uKey);
            free(current);
            oSymTable->length--;
            /* Return the value. */
            return value;
        }
        /* Otherwise, continue to the next binding. */
        previous = current;
        current = current->next;
    }
    /* Return NULL if the key does not exist. */
    return NULL;
}

/* SymTable_map: Applies the specified function to each binding in the 
symbol table. The client must pass a valid symbol table pointer and 
a non-NULL function pointer. The pvExtra parameter is passed through to 
the function, and can be NULL if not needed. */
void SymTable_map(SymTable_T oSymTable, void (*pfApply)(const char 
    *pcKey, void *pvValue, void *pvExtra), const void *pvExtra) {
    size_t i;
    Binding_T current;
    /* Check for NULL symbol table or function pointer. */
    assert(pfApply != NULL);
    if (oSymTable == NULL) {
        return;
    }
    /* Apply the function to each binding in the symbol table. */
    for (i = 0; i < auBucketCounts[oSymTable->bucket_ct_i]; ++i) {
        current = oSymTable->buckets[i];
        /* Apply the function to each binding in the current bucket. */
        while (current != NULL) {
            /* Apply the function to the current binding. */
            pfApply(current->uKey, current->uValue, (void *)pvExtra);
            current = current->next;
        }
    }
}