#include "SemanticAnalyzer.h"
#include "SymbolTable.h"
#include "../../support/logging/Logger.h"

static Logger* _logger = NULL;

static bool validateExpression(Expression* expr);
static bool validateCondition(Condition* cond);
static bool validateValue(Value* val);
static bool validateDeclaration(Declaration* decl);

static bool validateValue(Value* val) {
    if (val == NULL) return true;

    if (val->type == IDENTIFIER_VALUE) {
        if (!hasSymbol(val->identifier)) {
            logError(_logger, "Semantic Error: Variable '%s' is not defined.", val->identifier);
            return false;
        }
    } else if (val->type == EXPRESSION_VALUE) {
        return validateExpression(val->expression);
    } else if (val->type == LIST_VALUE) {
        Value* current = val->list;
        while (current != NULL) {
            if (!validateValue(current)) return false;
            current = current->next;
        }
    }
    return true;
}

static bool validateExpression(Expression* expr) {
    if (expr == NULL) return true;
    
    // Validate left and right branches
    if (!validateValue(expr->left)) return false;
    if (!validateValue(expr->right)) return false;

    // Both left and right must evaluate to numbers for arithmetic
    // Here we do a simplistic type check
    ValueType leftType = expr->left->type;
    ValueType rightType = expr->right->type;
    
    if (leftType == IDENTIFIER_VALUE) leftType = getSymbolType(expr->left->identifier);
    if (rightType == IDENTIFIER_VALUE) rightType = getSymbolType(expr->right->identifier);

    if (leftType == EXPRESSION_VALUE) leftType = NUMBER_VALUE; // expressions result in numbers
    if (rightType == EXPRESSION_VALUE) rightType = NUMBER_VALUE; // expressions result in numbers
    
    if (leftType != NUMBER_VALUE && leftType != PERCENTAGE_VALUE) {
        logError(_logger, "Semantic Error: Invalid type for arithmetic operation (Left operand).");
        return false;
    }
    if (rightType != NUMBER_VALUE && rightType != PERCENTAGE_VALUE) {
        logError(_logger, "Semantic Error: Invalid type for arithmetic operation (Right operand).");
        return false;
    }

    return true;
}

static bool validateCondition(Condition* cond) {
    if (cond == NULL) return true;

    if (cond->type == RELATIONAL_CONDITION) {
        if (!hasSymbol(cond->relational.field)) {
            logError(_logger, "Semantic Error: Variable '%s' is not defined in condition.", cond->relational.field);
            return false;
        }
        return validateValue(cond->relational.value);
    } else if (cond->type == NOT_CONDITION) {
        return validateCondition(cond->not_condition);
    } else if (cond->type == AND_CONDITION || cond->type == OR_CONDITION) {
        return validateCondition(cond->logical.left) && validateCondition(cond->logical.right);
    }

    return true;
}

static bool validateDeclaration(Declaration* decl) {
    if (decl == NULL) return true;

    if (decl->type == CONSTANT_DECLARATION) {
        if (!validateValue(decl->constant->value)) return false;
        
        ValueType type = decl->constant->value->type;
        if (type == EXPRESSION_VALUE) type = NUMBER_VALUE;
        if (type == IDENTIFIER_VALUE) type = getSymbolType(decl->constant->value->identifier);
        
        if (!putSymbol(decl->constant->name, type)) {
            logError(_logger, "Semantic Error: Constant '%s' is already defined.", decl->constant->name);
            return false;
        }
    } else if (decl->type == RULE_DECLARATION) {
        Condition* currentCond = decl->rule->conditions;
        while (currentCond != NULL) {
            if (!validateCondition(currentCond)) return false;
            currentCond = currentCond->next;
        }
    }
    
    // We can add further validation for Entity and Task declarations if needed
    // e.g., verifying their attributes against predefined lists

    return true;
}

CompilationStatus executeSemanticAnalysis(Program* program) {
    _logger = createLogger("SemanticAnalyzer");
    logDebugging(_logger, "Starting semantic analysis...");

    initializeSymbolTable();
    
    if (program == NULL || program->declarations == NULL) {
        logError(_logger, "Semantic Error: Program is empty.");
        destroySymbolTable();
        destroyLogger(_logger);
        return FAILED;
    }

    Declaration* current = program->declarations;
    while (current != NULL) {
        if (!validateDeclaration(current)) {
            destroySymbolTable();
            destroyLogger(_logger);
            return FAILED;
        }
        current = current->next;
    }

    logDebugging(_logger, "Semantic analysis completed successfully.");
    destroySymbolTable();
    destroyLogger(_logger);
    return SUCCEEDED;
}
