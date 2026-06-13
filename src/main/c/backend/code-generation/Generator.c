#include "Generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownGeneratorModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: Generator...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeGeneratorModule() {
	_logger = createLogger("Generator");
	return _shutdownGeneratorModule;
}

/** PRIVATE FUNCTIONS */

static void _generateProgram(Program * program);
static void _generateTxt(Program * program);
static void _generateJson(Program * program);
static void _generateValueJson(FILE * f, Value * value);
static void _generateConditionJson(FILE * f, Condition * condition);

static const char* _getTypeName(ValueType type) {
    switch (type) {
        case STRING_VALUE: return "STRING";
        case NUMBER_VALUE: return "NUMBER";
        case BOOLEAN_VALUE: return "BOOLEAN";
        case PERCENTAGE_VALUE: return "PERCENTAGE";
        case TIME_VALUE: return "TIME";
        case LIST_VALUE: return "LIST";
        case EXPRESSION_VALUE: return "EXPRESSION";
        case IDENTIFIER_VALUE: return "IDENTIFIER";
        default: return "UNKNOWN";
    }
}

/**
 * Generates human readable TXT.
 */
static void _generateTxt(Program * program) {
    FILE * f = fopen("afip_arba_configuration.txt", "w");
    if (!f) {
        logError(_logger, "Could not open afip_arba_configuration.txt for writing.");
        return;
    }
    fprintf(f, "==============================================\n");
    fprintf(f, " TAX-AS-CODE RULES ENGINE CONFIGURATION       \n");
    fprintf(f, "==============================================\n\n");

    Declaration* current = program->declarations;
    while (current != NULL) {
        if (current->type == CONSTANT_DECLARATION) {
            fprintf(f, "[CONSTANT] %s\n", current->constant->name);
        } else if (current->type == ENTITY_DECLARATION) {
            fprintf(f, "[ENTITY] %s\n", current->entity->name);
        } else if (current->type == RULE_DECLARATION) {
            fprintf(f, "[RULE] %s\n", current->rule->name);
        } else if (current->type == TASK_DECLARATION) {
            fprintf(f, "[TASK] %s\n", current->task->name);
        }
        current = current->next;
    }

    fprintf(f, "\n==============================================\n");
    fprintf(f, " GENERATED SUCCESSFULLY\n");
    fprintf(f, "==============================================\n");
    fclose(f);
    logDebugging(_logger, "TXT generation complete.");
}

static void _generateValueJson(FILE * f, Value * value) {
    if (value == NULL) {
        fprintf(f, "null");
        return;
    }
    switch (value->type) {
        case STRING_VALUE:
            fprintf(f, "\"%s\"", value->string);
            break;
        case NUMBER_VALUE:
        case PERCENTAGE_VALUE:
            fprintf(f, "%f", value->number);
            break;
        case BOOLEAN_VALUE:
            fprintf(f, "%s", value->boolean ? "true" : "false");
            break;
        case IDENTIFIER_VALUE:
            fprintf(f, "{\"type\": \"identifier\", \"value\": \"%s\"}", value->identifier);
            break;
        case TIME_VALUE:
            fprintf(f, "{\"type\": \"time\", \"value\": \"%s\"}", value->identifier);
            break;
        case LIST_VALUE:
            fprintf(f, "[");
            Value * item = value->list;
            while (item) {
                _generateValueJson(f, item);
                if (item->next) fprintf(f, ", ");
                item = item->next;
            }
            fprintf(f, "]");
            break;
        case EXPRESSION_VALUE:
            fprintf(f, "{\"type\": \"expression\", \"op\": %d, \"left\": ", value->expression->op);
            _generateValueJson(f, value->expression->left);
            fprintf(f, ", \"right\": ");
            _generateValueJson(f, value->expression->right);
            fprintf(f, "}");
            break;
    }
}

static void _generateConditionJson(FILE * f, Condition * condition) {
    if (condition == NULL) {
        fprintf(f, "null");
        return;
    }
    fprintf(f, "{");
    switch (condition->type) {
        case RELATIONAL_CONDITION:
            fprintf(f, "\"type\": \"relational\", \"field\": \"%s\", \"op\": %d, \"value\": ", condition->relational.field, condition->relational.op);
            _generateValueJson(f, condition->relational.value);
            break;
        case AND_CONDITION:
        case OR_CONDITION:
            fprintf(f, "\"type\": \"%s\", \"left\": ", condition->type == AND_CONDITION ? "and" : "or");
            _generateConditionJson(f, condition->logical.left);
            fprintf(f, ", \"right\": ");
            _generateConditionJson(f, condition->logical.right);
            break;
        case NOT_CONDITION:
            fprintf(f, "\"type\": \"not\", \"condition\": ");
            _generateConditionJson(f, condition->not_condition);
            break;
    }
    fprintf(f, "}");
}

/**
 * Generates machine readable JSON.
 */
static void _generateJson(Program * program) {
    FILE * f = fopen("tax_rules.json", "w");
    if (!f) {
        logError(_logger, "Could not open tax_rules.json for writing.");
        return;
    }
    fprintf(f, "{\n  \"rules\": [\n");
    
    bool first = true;
    Declaration* current = program->declarations;
    while (current != NULL) {
        if (current->type == RULE_DECLARATION) {
            if (!first) fprintf(f, ",\n");
            fprintf(f, "    { \"type\": \"RULE\", \"name\": \"%s\", \"conditions\": [", current->rule->name);
            Condition * cond = current->rule->conditions;
            bool firstCond = true;
            while(cond) {
                if (!firstCond) fprintf(f, ", ");
                _generateConditionJson(f, cond);
                firstCond = false;
                cond = cond->next;
            }
            fprintf(f, "] }");
            first = false;
        }
        current = current->next;
    }
    
    fprintf(f, "\n  ],\n  \"constants\": [\n");
    
    first = true;
    current = program->declarations;
    while (current != NULL) {
        if (current->type == CONSTANT_DECLARATION) {
            if (!first) fprintf(f, ",\n");
            fprintf(f, "    { \"type\": \"CONSTANT\", \"name\": \"%s\", \"value\": ", current->constant->name);
            _generateValueJson(f, current->constant->value);
            fprintf(f, " }");
            first = false;
        }
        current = current->next;
    }

    fprintf(f, "\n  ]\n}\n");
    fclose(f);
    logDebugging(_logger, "JSON generation complete.");
}

/**
 * Generates the output of the program.
 */
static void _generateProgram(Program * program) {
	logDebugging(_logger, "The AST has %s declarations.", program->declarations != NULL ? "some" : "no");
    if (program->declarations == NULL) return;
    
    logDebugging(_logger, "Generating TXT...");
    _generateTxt(program);

    logDebugging(_logger, "Generating JSON...");
    _generateJson(program);
}

/** PUBLIC FUNCTIONS */

void executeGenerator(CompilerState * compilerState) {
	logDebugging(_logger, "Generating final output...");
	_generateProgram(compilerState->abstractSyntaxtTree);
	logDebugging(_logger, "Generation is done.");
}
