%{

#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonActions.h"

/**
 * The error reporting function for Bison parser.
 *
 * @todo Add location to the grammar and "pushToken" API function.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Error-Reporting-Function.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Tracking-Locations.html
 */
void yyerror(const YYLTYPE * location, const char * message) {}

%}

// TaC Grammar configuration.
%define api.pure full
%define api.push-pull push
%define api.value.union.name SemanticValue
%define parse.error detailed
%locations

%union {
	/** Terminals. */
	double number;
	char * string;
	bool boolean;
	TokenLabel token;

	/** Non-terminals. */
	Value * value;
	Attribute * attribute;
	Entity * entity;
	Condition * condition;
	Rule * rule;
	Declaration * declaration;
	Program * program;
    RelationalOperator relOp;
}

/** Destructors for memory management. */
%destructor { destroyValue($$); } <value>
%destructor { destroyAttribute($$); } <attribute>
%destructor { destroyEntity($$); } <entity>
%destructor { destroyCondition($$); } <condition>
%destructor { destroyRule($$); } <rule>
%destructor { destroyDeclaration($$); } <declaration>

/** Terminals. */
%token <string> IDENTIFIER STRING
%token <number> NUMBER
%token <boolean> KW_VERDADERO KW_FALSO
%token <token> KW_CLIENTE KW_PROVEEDOR KW_MONOTRIBUTISTA KW_REGLA_FACTURACION KW_REGLA_PAGO
%token <token> OPEN_BRACE CLOSE_BRACE BULLET COLON
%token <token> OP_LE OP_GE OP_EQ
%token <token> UNKNOWN

/** Non-terminals. */
%type <program> program
%type <declaration> declarations declaration
%type <entity> entity_def
%type <rule> regla_def
%type <attribute> lista_atributos atributo
%type <condition> lista_condiciones condicion
%type <value> valor
%type <relOp> operador_relacional

%%

program: declarations										{ $$ = ProgramSemanticAction($1); }
	;

declarations: declaration									{ $$ = $1; }
    | declarations declaration                              { $$ = DeclarationListSemanticAction($1, $2); }
    ;

declaration: entity_def                                     { $$ = EntityDeclarationSemanticAction($1); }
    | regla_def                                             { $$ = RuleDeclarationSemanticAction($1); }
    ;

entity_def: KW_CLIENTE STRING OPEN_BRACE lista_atributos CLOSE_BRACE { $$ = EntitySemanticAction($2, $4); }
    | KW_PROVEEDOR STRING OPEN_BRACE lista_atributos CLOSE_BRACE     { $$ = EntitySemanticAction($2, $4); }
    ;

lista_atributos: atributo                                   { $$ = $1; }
    | lista_atributos atributo                              { $$ = AttributeListSemanticAction($1, $2); }
    ;

atributo: IDENTIFIER COLON valor                            { $$ = AttributeSemanticAction($1, $3); }
    ;

regla_def: KW_REGLA_FACTURACION STRING OPEN_BRACE lista_condiciones CLOSE_BRACE { $$ = RuleSemanticAction($2, $4); }
    | KW_REGLA_PAGO STRING OPEN_BRACE lista_condiciones CLOSE_BRACE             { $$ = RuleSemanticAction($2, $4); }
    ;

lista_condiciones: condicion                                { $$ = $1; }
    | lista_condiciones condicion                           { $$ = ConditionListSemanticAction($1, $2); }
    ;

condicion: BULLET IDENTIFIER operador_relacional valor       { $$ = ConditionSemanticAction($2, $3, $4); }
    ;

operador_relacional: OP_LE                                  { $$ = LE; }
    | OP_GE                                                 { $$ = GE; }
    | OP_EQ                                                 { $$ = EQ; }
    ;

valor: STRING                                               { $$ = StringValueSemanticAction($1); }
    | NUMBER                                                { $$ = NumberValueSemanticAction($1); }
    | KW_VERDADERO                                          { $$ = BooleanValueSemanticAction(true); }
    | KW_FALSO                                              { $$ = BooleanValueSemanticAction(false); }
    ;

%%

