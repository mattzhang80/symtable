/*symtablehash implements a Symbol Table ADT using a hash table, with
functions to create an empty symbol table, free a symbol table, get the 
length of a symbol table, put a key-value pair into the symbol table, 
replace the value of a key-value with another value, 
check if a symbol table contains a key, gets the value from a key-value, 
removes a key-value, and applies a function to all key-value Bindings*/
/*Author: Matthew Zhang*/

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
/* Number of elements in the auBucketCounts array. */
static const size_t numBucketCounts = 
    sizeof(auBucketCounts)/sizeof(auBucketCounts[0]);

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

/* SymTable_resize: Resizes the symbol table to the next bucket count.
Client must input a valid oSymTable, and the function will return 
returns 1 if successful, while it returns 0 if the symbol table has 
reached the max bucket count or is full or fails to allocate memory*/
static int SymTable_resize(SymTable_T oSymTable) {
    size_t i;
    Binding_T curr, next;
    Binding_T *new_buckets;
    size_t new_bucket_ct;
    /* Check for NULL symbol table. */
    assert(oSymTable != NULL);
    /* Check if the symbol table is full. */
    if (oSymTable->length != auBucketCounts[oSymTable->bucket_ct_i]) {
        return 0;
    }
    /* Check if the symbol table has reached the maximum bucket count. */
    if (oSymTable->bucket_ct_i == numBucketCounts - 1) {
        return 0;
    }
    /* Allocate memory for the new buckets array. */
    new_bucket_ct = auBucketCounts[oSymTable->bucket_ct_i + 1];
    new_buckets = (Binding_T *)calloc(new_bucket_ct, sizeof(Binding_T));
    /* Check for memory allocation failure. */
    if (new_buckets == NULL) {
        return 0;
    }
    /* Rehash the bindings in the symbol table. */
    for (i = 0; i < auBucketCounts[oSymTable->bucket_ct_i]; ++i) {
        curr = oSymTable->buckets[i];
        /* Rehash the bindings in the current bucket. */
        while (curr != NULL) {
            next = curr->next;
            /* Add the binding to the new buckets array. */
            curr->next = new_buckets[SymTable_hash(curr->uKey, 
                new_bucket_ct)];
            /* Update the new bucket pointer. */
            new_buckets[SymTable_hash(curr->uKey, new_bucket_ct)]= curr;
            curr = next;
        }
    }
    /* Free the old buckets array and update the symbol table. */
    free(oSymTable->buckets);
    oSymTable->buckets = new_buckets;
    oSymTable->bucket_ct_i++;
    /* Return 1 if successful. */
    return 1;
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
    Binding_T curr, next;
    /* Check for NULL symbol table. */
    if (oSymTable == NULL) {
        return;
    }
    /* Free all bindings in the symbol table. */
    for (i = 0; i < auBucketCounts[oSymTable->bucket_ct_i]; ++i) {
        curr = oSymTable->buckets[i];
        /* Free all bindings in the current bucket. */
        while (curr != NULL) {
            next = curr->next;
            free(curr->uKey);
            free(curr);
            curr = next;
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
    Binding_T newB;
    size_t hashIndex;
    char *copyKey;

    /* Check for NULL symbol table, key, or value. */
    assert(pcKey != NULL);
    if (oSymTable == NULL) {
        return 0;
    }
    /* Check if key already exists. */
    if (SymTable_contains(oSymTable, pcKey)) {
        return 0;
    }
    /* Hash the key and copy it. */
    hashIndex = SymTable_hash(pcKey, auBucketCounts[oSymTable->bucket_ct_i]);
    copyKey = (char *)malloc(strlen(pcKey) + 1);
    /* Check for memory allocation failure. */
    if (copyKey == NULL) return 0;
    /* Copy the key. */
    strcpy(copyKey, pcKey);
    /* Allocate memory for the new binding. */
    newB = (Binding_T)malloc(sizeof(struct Binding));
    /* Check for memory allocation failure. */
    if (newB == NULL) {
        free(copyKey);
        return 0;
    }
    /* Add new binding to the symbol table. */
    newB->uKey = copyKey;
    newB->uValue = (void *)pvValue;
    newB->next = oSymTable->buckets[hashIndex];
    oSymTable->buckets[hashIndex] = newB;
    oSymTable->length++;
    /* If the symbol table is full, resize it. */
    if (oSymTable->length == auBucketCounts[oSymTable->bucket_ct_i]) {
        SymTable_resize(oSymTable);
    }
    /* Return 1 if successful. */
    return 1;
}

/* SymTable_replace: If a binding with the specified key exists, 
replaces its value and returns the old value. Otherwise, returns NULL. 
The client must pass valid symbol table and key pointers. */
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue) {
    size_t hashIndex;
    Binding_T curr;
    void *oldVal;
    /* Check for NULL symbol table, key, or value. */
    assert(pcKey != NULL);
    if (oSymTable == NULL) {
        return NULL;
    }
    /* Hash the key. */
    hashIndex = SymTable_hash(pcKey, auBucketCounts[oSymTable->bucket_ct_i]);
    curr = oSymTable->buckets[hashIndex];
    /* Replace the value if the key exists. */
    while (curr != NULL) {
        /* If key exists, replace the value and return the old value. */
        if (strcmp(curr->uKey, pcKey) == 0) {
            oldVal = curr->uValue;
            curr->uValue = (void *)pvValue;
            return oldVal;
        }
        curr = curr->next;
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
    Binding_T curr, prev;
    void *val;
    /* Check for NULL symbol table or key. */
    assert(pcKey != NULL);
    if (oSymTable == NULL) {
        return NULL;
    }
    /* Check if key exists. */
    index = SymTable_hash(pcKey, auBucketCounts[oSymTable->bucket_ct_i]);
    curr = oSymTable->buckets[index];
    prev = NULL;
    /* Remove the binding if the key exists. */
    while (curr != NULL) {
        /* If the key exists, remove the binding and return the value. */
        if (strcmp(curr->uKey, pcKey) == 0) {
            /* If the binding is the first in the bucket, update the
            bucket pointer. */
            if (prev == NULL) {
                oSymTable->buckets[index] = curr->next;
            } 
            /* Otherwise, update the previous binding's next pointer. */
            else {
                prev->next = curr->next;
            }
            /* Free the key and binding, and return the value. */
            val = curr->uValue;
            free(curr->uKey);
            free(curr);
            oSymTable->length--;
            /* Return the value. */
            return val;
        }
        /* Otherwise, continue to the next binding. */
        prev = curr;
        curr = curr->next;
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
            (*pfApply)(current->uKey, current->uValue, (void *)pvExtra);
            current = current->next;
        }
    }
}