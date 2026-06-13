#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonParser.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState);

/**
 * Bison semantic actions.
 */

Program * ProgramSemanticAction(Declaration * declarations);
Declaration * DeclarationListSemanticAction(Declaration * list, Declaration * next);
Declaration * EntityDeclarationSemanticAction(Entity * entity);
Declaration * RuleDeclarationSemanticAction(Rule * rule);
Declaration * TaskDeclarationSemanticAction(Task * task);
Declaration * ConstantDeclarationSemanticAction(char * name, Value * value);
Entity * EntitySemanticAction(EntityType type, char * name, Attribute * attributes);
Attribute * AttributeListSemanticAction(Attribute * list, Attribute * next);
Attribute * AttributeSemanticAction(char * name, Value * value);
Rule * RuleSemanticAction(RuleType type, char * name, Condition * conditions);
Task * TaskSemanticAction(TaskType type, char * name, Attribute * attributes);
Condition * ConditionListSemanticAction(Condition * list, Condition * next);
Condition * ConditionSemanticAction(char * field, RelationalOperator op, Value * value);
Condition * RelationalConditionSemanticAction(char * field, RelationalOperator op, Value * value);
Condition * LogicalConditionSemanticAction(ConditionType type, Condition * left, Condition * right);
Condition * NotConditionSemanticAction(Condition * condition);
Value * StringValueSemanticAction(char * string);
Value * NumberValueSemanticAction(double number);
Value * BooleanValueSemanticAction(bool boolean);
Value * PercentageValueSemanticAction(double number);
Value * TimeValueSemanticAction(char * identifier);
Value * ListValueSemanticAction(Value * list);
Value * ValueListSemanticAction(Value * list, Value * next);
Value * ExpressionSemanticAction(ExpressionType op, Value * left, Value * right);
Value * IdentifierValueSemanticAction(char * identifier);


#endif
