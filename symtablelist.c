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
    struct Binding *head;
    size_t length;
};

SymTable_T SymTable_new(void) {
    SymTable_T oSymTable = malloc(sizeof(struct SymTable));
    if (!oSymTable) {
        return NULL;  
    }
    oSymTable->head = NULL;
    oSymTable->length = 0;
    return oSymTable;
}

void SymTable_free(SymTable_T oSymTable) {
    if (!oSymTable) return;  // Added check for NULL pointer

    struct Binding *current = oSymTable->head;
    struct Binding *next;

    while (current != NULL) {
        next = current->next;
        free(current->uKey);
        free(current);
        current = next;
    }
    free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable) {
    assert(oSymTable != NULL);
    return oSymTable->length;
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue) {
    assert(oSymTable != NULL && pcKey != NULL);

    if (SymTable_contains(oSymTable, pcKey)) {
        return 0;
    }

    char *keyCopy = strdup(pcKey);  // Using strdup to simplify code
    if (!keyCopy) return 0;

    struct Binding *newB = malloc(sizeof(struct Binding));
    if (!newB) {
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

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue) {
    assert(oSymTable != NULL && pcKey != NULL);

    struct Binding *curr = oSymTable->head;

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

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    assert(oSymTable != NULL && pcKey != NULL);

    struct Binding *curr = oSymTable->head;

    while (curr != NULL) {
        if (strcmp(curr->uKey, pcKey) == 0) {
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
    assert(oSymTable != NULL && pcKey != NULL);

    struct Binding *curr = oSymTable->head;

    while (curr != NULL) {
        if (strcmp(curr->uKey, pcKey) == 0) {
            return curr->uValue;
        }
        curr = curr->next;
    }
    return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
    assert(oSymTable != NULL && pcKey != NULL);

    struct Binding *curr = oSymTable->head;
    struct Binding *prev = NULL;

    while (curr != NULL) {
        if (strcmp(curr->uKey, pcKey) == 0) {
            if (prev == NULL) {
                oSymTable->head = curr->next;
            } else {
                prev->next = curr->next;
            }
            void *value = curr->uValue;
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

void SymTable_map(SymTable_T oSymTable, void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), const void *pvExtra) {
    assert(oSymTable != NULL && pfApply != NULL);

    struct Binding *curr = oSymTable->head;

    while (curr != NULL) {
        pfApply(curr->uKey, curr->uValue, (void *)pvExtra);
        curr = curr->next;
    }
}