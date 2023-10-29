#include <stdlib.h>
#include "symtable.h"
#include "assert.h"

/*Binding is a node of the linked list that is the SymTable. It contains 
a keymand a value, and a pointer to the next node in the list.*/
struct Binding {
    char *uKey;
    void *uValue;
    struct Binding *next;
};
/*SymTable is a linked list of Bindings. It contains a pointer to the 
first node in the list, and the length of the list.*/
struct SymTable {
    struct Binding *head;
    size_t length;
};
/*SymTable_new creates a new SymTable and returns a pointer to it.*/
SymTable_T SymTable_new(void) {
    SymTable_T oSymTable = (SymTable_T)sizeof(struct SymTable);
    assert(oSymTable != NULL);
    oSymTable->head = NULL;
    oSymTable->length = 0;
    return oSymTable;
};

void SymTable_free(SymTable_T oSymTable) {
    assert(oSymTable != NULL);
    struct Binding *current = oSymTable -> head;
    struct Binding *next;

    while (current != NULL) {
        next = current -> next;
        free(current -> uKey);
        free(current);
        current = next;
    }
};

size_t SymTable_getLength(SymTable_T oSymTable) {
    assert(oSymTable != NULL);
    return oSymTable -> length;
};

int SymTable_put(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue) {
        assert(oSymTable != NULL);
        assert(pcKey != NULL);

        if(oSymTable_contains(oSymTable, pcKey)) {
            return 0;
        }

        struct Binding *newB = (struct Binging *)malloc(sizeof(struct 
        Binding));
        assert(newB != NULL);

        newB -> uKey = (char *)malloc(strlen(pcKey) + 1);
        assert(newB -> uKey != NULL);
        strcpy(newB -> uKey, pcKey);

        newB -> uValue = (void *)pvValue;
        newB -> next = oSymTable -> head;

        oSymTable -> head = newB;
        oSymTable -> length++;

        return 1;
    };

void *SymTable_replace(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue) {
        assert(oSymTable != NULL);
        assert(pcKey != NULL);  

        struct Binding *curr = oSymTable -> head;

        while(curr != NULL) {
            if(strcmp(curr -> uKey, pcKey) == 0) {
                void *prevValue = curr -> uValue;
                curr -> uValue = (void *)pvValue;
                return prevValue;
            }
            else {
                curr = curr -> next;
            }
        }
    }; 

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    struct Binding *curr = oSymTable -> head;

    while(curr != NULL) {
        if(strcmp (curr -> uKey, pcKey) == 0) {
            return 1;
        }
        curr = curr -> next;
    }
};

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    struct Binding *curr = oSymTable -> head;

    while(curr != NULL) {
        if(strcmp(curr ->uKey, pcKey) == 0) {
            return curr -> uValue;
        }
        curr = curr -> next;
    }
    return NULL;
};

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    struct Binding *curr = oSymTable -> head;
    struct Binding *prev = NULL;

    while(curr != NULL) {
        if(strcmp(curr -> uKey, pcKey) == 0) {
            if(prev == NULL) {
                oSymTable -> head = curr -> next;
            }
            else {
                prev -> next = curr -> next;
            }
        }
        void *prevValue = curr -> uValue;
        free(curr ->uKey);
        free(curr -> uValue);
        oSymTable -> length--;
        return prevValue;
    }
};

void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra) {
        assert(oSymTable != NULL);
        assert(pfApply != NULL);

        struct Binding *curr = oSymTable -> head;
        while(curr != NULL) {
            (*pfApply)(curr -> uKey, curr -> uValue, pvExtra);
            curr = curr -> next;
        }
    };
