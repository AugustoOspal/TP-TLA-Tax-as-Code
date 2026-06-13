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
        switch (value->type) {
            case STRING_VALUE:
            case TIME_VALUE:
                if (value->string != NULL) {
                    free(value->string);
                }
                break;
            case IDENTIFIER_VALUE:
                if (value->identifier != NULL) {
                    free(value->identifier);
                }
                break;
            case LIST_VALUE:
                destroyValue(value->list);
                break;
            case EXPRESSION_VALUE:
                destroyExpression(value->expression);
                break;
            default:
                break;
        }
        destroyValue(value->next);
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
        switch (condition->type) {
            case RELATIONAL_CONDITION:
                if (condition->relational.field != NULL) {
                    free(condition->relational.field);
                }
                destroyValue(condition->relational.value);
                break;
            case AND_CONDITION:
            case OR_CONDITION:
                destroyCondition(condition->logical.left);
                destroyCondition(condition->logical.right);
                break;
            case NOT_CONDITION:
                destroyCondition(condition->not_condition);
                break;
        }
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

void destroyTask(Task * task) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (task != NULL) {
        if (task->name != NULL) {
            free(task->name);
        }
        destroyAttribute(task->attributes);
        destroyTask(task->next);
        free(task);
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
            case TASK_DECLARATION:
                destroyTask(declaration->task);
                break;
            case CONSTANT_DECLARATION:
                destroyConstantDeclaration(declaration->constant);
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

void destroyExpression(Expression * expression) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (expression != NULL) {
        destroyValue(expression->left);
        destroyValue(expression->right);
        free(expression);
    }
}

void destroyConstantDeclaration(ConstantDeclaration * constant) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (constant != NULL) {
        if (constant->name != NULL) {
            free(constant->name);
        }
        destroyValue(constant->value);
        free(constant);
    }
}

