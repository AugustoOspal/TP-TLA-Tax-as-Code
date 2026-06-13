#ifndef SYMBOL_TABLE_HEADER
#define SYMBOL_TABLE_HEADER

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"

void initializeSymbolTable(void);
void destroySymbolTable(void);
bool putSymbol(const char* identifier, ValueType type);
bool hasSymbol(const char* identifier);
ValueType getSymbolType(const char* identifier);

#endif
