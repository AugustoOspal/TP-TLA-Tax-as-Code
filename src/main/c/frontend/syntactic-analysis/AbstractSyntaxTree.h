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
    BOOLEAN_VALUE,
    PERCENTAGE_VALUE,
    TIME_VALUE,
    LIST_VALUE,
    EXPRESSION_VALUE,
    IDENTIFIER_VALUE
} ValueType;

typedef enum ExpressionType {
    ADD,
    SUB,
    MUL,
    DIV
} ExpressionType;

struct Expression {
    ExpressionType op;
    struct Value * left;
    struct Value * right;
};

typedef struct Expression Expression;

typedef struct Value {
    ValueType type;
    union {
        char * string;
        double number;
        bool boolean;
        struct Value * list;
        struct Expression * expression;
        char * identifier;
    };
    struct Value * next;
} Value;

typedef struct Attribute {
    char * name;
    Value * value;
    struct Attribute * next;
} Attribute;

typedef enum EntityType {
    CLIENTE,
    PROVEEDOR,
    MONOTRIBUTISTA
} EntityType;

typedef struct Entity {
    EntityType type;
    char * name;
    Attribute * attributes;
    struct Entity * next;
} Entity;

typedef enum RelationalOperator {
    LE,
    GE,
    EQ
} RelationalOperator;

typedef enum ConditionType {
    RELATIONAL_CONDITION,
    AND_CONDITION,
    OR_CONDITION,
    NOT_CONDITION
} ConditionType;

typedef struct Condition {
    ConditionType type;
    union {
        struct {
            char * field;
            RelationalOperator op;
            Value * value;
        } relational;
        struct {
            struct Condition * left;
            struct Condition * right;
        } logical;
        struct Condition * not_condition;
    };
    struct Condition * next;
} Condition;

typedef enum RuleType {
    FACTURACION,
    PAGO,
    EXENCION
} RuleType;

typedef struct Rule {
    RuleType type;
    char * name;
    Condition * conditions;
    struct Rule * next;
} Rule;

typedef enum TaskType {
    SINCRONIZACION,
    CIERRE
} TaskType;

typedef struct Task {
    TaskType type;
    char * name;
    Attribute * attributes;
    struct Task * next;
} Task;

typedef struct ConstantDeclaration {
    char * name;
    Value * value;
} ConstantDeclaration;

typedef enum DeclarationType {
    ENTITY_DECLARATION,
    RULE_DECLARATION,
    TASK_DECLARATION,
    CONSTANT_DECLARATION
} DeclarationType;

typedef struct Declaration {
    DeclarationType type;
    union {
        Entity * entity;
        Rule * rule;
        Task * task;
        ConstantDeclaration * constant;
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
void destroyTask(Task * task);
void destroyDeclaration(Declaration * declaration);
void destroyExpression(Expression * expression);
void destroyConstantDeclaration(ConstantDeclaration * constant);
void destroyProgram(Program * program);

#endif

