#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: AbstractSyntaxTree...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntaxTree");
	return _shutdownAbstractSyntaxTreeModule;
}

/* PUBLIC FUNCTIONS */

void destroyValue(Value * value) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (value != NULL) {
        if (value->type == STRING_VALUE && value->string != NULL) {
            free(value->string);
        }
        free(value);
    }
}

void destroyAttribute(Attribute * attribute) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (attribute != NULL) {
        if (attribute->name != NULL) {
            free(attribute->name);
        }
        destroyValue(attribute->value);
        destroyAttribute(attribute->next);
        free(attribute);
    }
}

void destroyEntity(Entity * entity) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (entity != NULL) {
        if (entity->name != NULL) {
            free(entity->name);
        }
        destroyAttribute(entity->attributes);
        destroyEntity(entity->next);
        free(entity);
    }
}

void destroyCondition(Condition * condition) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (condition != NULL) {
        if (condition->field != NULL) {
            free(condition->field);
        }
        destroyValue(condition->value);
        destroyCondition(condition->next);
        free(condition);
    }
}

void destroyRule(Rule * rule) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (rule != NULL) {
        if (rule->name != NULL) {
            free(rule->name);
        }
        destroyCondition(rule->conditions);
        destroyRule(rule->next);
        free(rule);
    }
}

void destroyDeclaration(Declaration * declaration) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (declaration != NULL) {
        switch (declaration->type) {
            case ENTITY_DECLARATION:
                destroyEntity(declaration->entity);
                break;
            case RULE_DECLARATION:
                destroyRule(declaration->rule);
                break;
        }
        destroyDeclaration(declaration->next);
        free(declaration);
    }
}

void destroyProgram(Program * program) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (program != NULL) {
        destroyDeclaration(program->declarations);
        free(program);
    }
}

