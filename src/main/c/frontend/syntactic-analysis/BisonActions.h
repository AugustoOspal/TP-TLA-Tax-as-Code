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
Entity * EntitySemanticAction(char * name, Attribute * attributes);
Attribute * AttributeListSemanticAction(Attribute * list, Attribute * next);
Attribute * AttributeSemanticAction(char * name, Value * value);
Rule * RuleSemanticAction(char * name, Condition * conditions);
Condition * ConditionListSemanticAction(Condition * list, Condition * next);
Condition * ConditionSemanticAction(char * field, RelationalOperator op, Value * value);
Value * StringValueSemanticAction(char * string);
Value * NumberValueSemanticAction(double number);
Value * BooleanValueSemanticAction(bool boolean);


#endif
