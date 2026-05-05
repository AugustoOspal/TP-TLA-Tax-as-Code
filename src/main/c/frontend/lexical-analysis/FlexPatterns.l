%top{

#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/TokenLabel.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"

}

%{

/**
 * @see https://westes.github.io/flex/manual/Definitions-Section.html#Definitions-Section
 */

extern CompilationStatus TokenLexemeAction(TokenLabel label);
extern CompilationStatus IdentifierLexemeAction();
extern CompilationStatus NumberLexemeAction();
extern CompilationStatus StringLexemeAction();
extern CompilationStatus BooleanLexemeAction(bool value);
extern CompilationStatus EOFLexemeAction();
extern CompilationStatus IgnoredLexemeAction();
extern CompilationStatus UnknownLexemeAction();

%}

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */
whitespace			[ \t\n\r]+

%%

%{

/**
 * @see https://westes.github.io/flex/manual/Rules-Section.html#Rules-Section
 */

%}

"cliente"							{ return TokenLexemeAction(KW_CLIENTE); }
"proveedor"							{ return TokenLexemeAction(KW_PROVEEDOR); }
"monotributista"					{ return TokenLexemeAction(KW_MONOTRIBUTISTA); }
"regla_facturacion"					{ return TokenLexemeAction(KW_REGLA_FACTURACION); }
"regla_pago"						{ return TokenLexemeAction(KW_REGLA_PAGO); }
"regla_exencion"					{ return TokenLexemeAction(KW_REGLA_EXENCION); }
"sincronizacion_padrones"			{ return TokenLexemeAction(KW_SINCRONIZACION_PADRONES); }
"cierre_mensual"					{ return TokenLexemeAction(KW_CIERRE_MENSUAL); }
"ultimo_dia_mes"					{ return TokenLexemeAction(KW_ULTIMO_DIA_MES); }

"verdadero"							{ return BooleanLexemeAction(true); }
"falso"								{ return BooleanLexemeAction(false); }

"menor"[[:space:]]+"o"[[:space:]]+"igual"[[:space:]]+"a"	{ return TokenLexemeAction(OP_LE); }
"mayor"[[:space:]]+"o"[[:space:]]+"igual"[[:space:]]+"a"	{ return TokenLexemeAction(OP_GE); }
"igual"[[:space:]]+"a"				{ return TokenLexemeAction(OP_EQ); }

"{"									{ return TokenLexemeAction(OPEN_BRACE); }
"}"									{ return TokenLexemeAction(CLOSE_BRACE); }
"["									{ return TokenLexemeAction(OPEN_BRACKET); }
"]"									{ return TokenLexemeAction(CLOSE_BRACKET); }
"-"									{ return TokenLexemeAction(BULLET); }
":"									{ return TokenLexemeAction(COLON); }
"%"									{ return TokenLexemeAction(PERCENT); }
","									{ return TokenLexemeAction(COMMA); }

[[:alpha:]_][[:alnum:]_]*			{ return IdentifierLexemeAction(); }
[[:digit:]]+(\.[[:digit:]]+)?		{ return NumberLexemeAction(); }
\"[^"]*\"							{ return StringLexemeAction(); }

("#"|"//").*						{ return IgnoredLexemeAction(); }

{whitespace}						{ return IgnoredLexemeAction(); }
.									{ return UnknownLexemeAction(); }

<<EOF>>								{ return EOFLexemeAction(); }


%%

/**
 * @see https://westes.github.io/flex/manual/User-Code-Section.html#User-Code-Section
 */

#include "FlexActions.h"
#include "FlexExport.h"
