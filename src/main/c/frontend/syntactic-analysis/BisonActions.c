#include "BisonActions.h"

/* MODULE INTERNAL STATE */

static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownBisonActionsModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: BisonActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
	_compilerState = NULL;
}

ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState) {
	_compilerState = compilerState;
	_logger = createLogger("BisonActions");
	return _shutdownBisonActionsModule;
}

/* IMPORTED FUNCTIONS */

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

/* PUBLIC FUNCTIONS */

Program * ProgramSemanticAction(Declaration * declarations) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->declarations = declarations;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}

Declaration * DeclarationListSemanticAction(Declaration * list, Declaration * next) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
    Declaration * current = list;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = next;
    return list;
}

Declaration * EntityDeclarationSemanticAction(Entity * entity) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
    Declaration * declaration = calloc(1, sizeof(Declaration));
    declaration->type = ENTITY_DECLARATION;
    declaration->entity = entity;
    return declaration;
}

Declaration * RuleDeclarationSemanticAction(Rule * rule) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
    Declaration * declaration = calloc(1, sizeof(Declaration));
    declaration->type = RULE_DECLARATION;
    declaration->rule = rule;
    return declaration;
}

Entity * EntitySemanticAction(char * name, Attribute * attributes) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
    Entity * entity = calloc(1, sizeof(Entity));
    entity->name = name;
    entity->attributes = attributes;
    return entity;
}

Attribute * AttributeListSemanticAction(Attribute * list, Attribute * next) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
    Attribute * current = list;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = next;
    return list;
}

Attribute * AttributeSemanticAction(char * name, Value * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
    Attribute * attribute = calloc(1, sizeof(Attribute));
    attribute->name = name;
    attribute->value = value;
    return attribute;
}

Rule * RuleSemanticAction(char * name, Condition * conditions) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
    Rule * rule = calloc(1, sizeof(Rule));
    rule->name = name;
    rule->conditions = conditions;
    return rule;
}

Condition * ConditionListSemanticAction(Condition * list, Condition * next) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
    Condition * current = list;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = next;
    return list;
}

Condition * ConditionSemanticAction(char * field, RelationalOperator op, Value * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
    Condition * condition = calloc(1, sizeof(Condition));
    condition->field = field;
    condition->op = op;
    condition->value = value;
    return condition;
}

Value * StringValueSemanticAction(char * string) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
    Value * value = calloc(1, sizeof(Value));
    value->type = STRING_VALUE;
    value->string = string;
    return value;
}

Value * NumberValueSemanticAction(double number) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
    Value * value = calloc(1, sizeof(Value));
    value->type = NUMBER_VALUE;
    value->number = number;
    return value;
}

Value * BooleanValueSemanticAction(bool boolean) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
    Value * value = calloc(1, sizeof(Value));
    value->type = BOOLEAN_VALUE;
    value->boolean = boolean;
    return value;
}

