#include "SymbolTable.h"
#include <stdio.h>

typedef struct SymbolNode {
    char* identifier;
    ValueType type;
    struct SymbolNode* next;
} SymbolNode;

static SymbolNode* head = NULL;

void initializeSymbolTable(void) {
    head = NULL;
    // Inject predefined domain variables (these are resolved at runtime by the execution engine)
    putSymbol("monto", NUMBER_VALUE);
    putSymbol("cuit", STRING_VALUE);
    putSymbol("condicion_iva", STRING_VALUE);
    putSymbol("ingresos_brutos", NUMBER_VALUE);
    putSymbol("es_agente_retencion", BOOLEAN_VALUE);
    putSymbol("constancia_activa", BOOLEAN_VALUE);
    putSymbol("agente_retencion", BOOLEAN_VALUE);
    putSymbol("score", NUMBER_VALUE);
    putSymbol("facturacion", NUMBER_VALUE);
    putSymbol("es_pyme", BOOLEAN_VALUE);
    putSymbol("es_monotributista", BOOLEAN_VALUE);
    putSymbol("alicuota_iibb", PERCENTAGE_VALUE);
    putSymbol("concepto", STRING_VALUE);
}

void destroySymbolTable(void) {
    SymbolNode* current = head;
    while (current != NULL) {
        SymbolNode* next = current->next;
        free(current->identifier);
        free(current);
        current = next;
    }
    head = NULL;
}

bool putSymbol(const char* identifier, ValueType type) {
    if (hasSymbol(identifier)) {
        return false; // Symbol already exists
    }
    SymbolNode* newNode = (SymbolNode*)malloc(sizeof(SymbolNode));
    if (!newNode) return false;
    
    newNode->identifier = strdup(identifier);
    newNode->type = type;
    newNode->next = head;
    head = newNode;
    return true;
}

bool hasSymbol(const char* identifier) {
    SymbolNode* current = head;
    while (current != NULL) {
        if (strcmp(current->identifier, identifier) == 0) {
            return true;
        }
        current = current->next;
    }
    return false;
}

ValueType getSymbolType(const char* identifier) {
    SymbolNode* current = head;
    while (current != NULL) {
        if (strcmp(current->identifier, identifier) == 0) {
            return current->type;
        }
        current = current->next;
    }
    return -1; // Not found
}
