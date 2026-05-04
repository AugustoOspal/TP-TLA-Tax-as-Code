#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdlib.h>
#include <stdbool.h>

/** Initialize module's internal state. */
ModuleDestructor initializeAbstractSyntaxTreeModule();

/**
 * Type definitions for TaC DSL.
 */

typedef enum ValueType {
    STRING_VALUE,
    NUMBER_VALUE,
    BOOLEAN_VALUE
} ValueType;

typedef struct Value {
    ValueType type;
    union {
        char * string;
        double number;
        bool boolean;
    };
} Value;

typedef struct Attribute {
    char * name;
    Value * value;
    struct Attribute * next;
} Attribute;

typedef struct Entity {
    char * name;
    Attribute * attributes;
    struct Entity * next;
} Entity;

typedef enum RelationalOperator {
    LE,
    GE,
    EQ
} RelationalOperator;

typedef struct Condition {
    char * field;
    RelationalOperator op;
    Value * value;
    struct Condition * next;
} Condition;

typedef struct Rule {
    char * name;
    Condition * conditions;
    struct Rule * next;
} Rule;

typedef enum DeclarationType {
    ENTITY_DECLARATION,
    RULE_DECLARATION
} DeclarationType;

typedef struct Declaration {
    DeclarationType type;
    union {
        Entity * entity;
        Rule * rule;
    };
    struct Declaration * next;
} Declaration;

struct Program {
    Declaration * declarations;
};

typedef struct Program Program;

/**
 * Node recursive destructors.
 */

void destroyValue(Value * value);
void destroyAttribute(Attribute * attribute);
void destroyEntity(Entity * entity);
void destroyCondition(Condition * condition);
void destroyRule(Rule * rule);
void destroyDeclaration(Declaration * declaration);
void destroyProgram(Program * program);

#endif

