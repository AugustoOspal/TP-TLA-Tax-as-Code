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
    Task * task;
	Declaration * declaration;
	Program * program;
    RelationalOperator relOp;
}

/** Destructors for memory management. */
%destructor { free($$); } <string>
%destructor { destroyValue($$); } <value>
%destructor { destroyAttribute($$); } <attribute>
%destructor { destroyEntity($$); } <entity>
%destructor { destroyCondition($$); } <condition>
%destructor { destroyRule($$); } <rule>
%destructor { destroyTask($$); } <task>
%destructor { destroyDeclaration($$); } <declaration>

/** Terminals. */
%token <string> IDENTIFIER STRING
%token <number> NUMBER
%token <boolean> KW_VERDADERO KW_FALSO
%token <token> KW_CLIENTE KW_PROVEEDOR KW_MONOTRIBUTISTA KW_DEFINIR
%token <token> KW_REGLA_FACTURACION KW_REGLA_PAGO KW_REGLA_EXENCION
%token <token> KW_SINCRONIZACION_PADRONES KW_CIERRE_MENSUAL
%token <token> KW_ULTIMO_DIA_MES
%token <token> OPEN_BRACE CLOSE_BRACE OPEN_BRACKET CLOSE_BRACKET OP_SUB COLON PERCENT COMMA
%token <token> OPEN_PAREN CLOSE_PAREN
%token <token> KW_Y KW_O KW_NO
%token <token> OP_ADD OP_MUL OP_DIV
%token <token> OP_LE OP_GE OP_EQ
%token <token> UNKNOWN

%left KW_O
%left KW_Y
%right KW_NO
%left OP_ADD OP_SUB
%left OP_MUL OP_DIV
%left PREC_CONDICION

/** Non-terminals. */
%type <program> program
%type <declaration> declarations declaration
%type <entity> entity_def
%type <rule> regla_def
%type <task> task_def
%type <declaration> def_constante
%type <attribute> lista_atributos atributo
%type <condition> lista_condiciones condicion condicion_logica
%type <value> valor lista_valores expresion
%type <relOp> operador_relacional

%%

program: declarations										{ $$ = ProgramSemanticAction($1); }
	;

declarations: declaration									{ $$ = $1; }
    | declarations declaration                              { $$ = DeclarationListSemanticAction($1, $2); }
    ;

declaration: entity_def                             { $$ = EntityDeclarationSemanticAction($1); }
    | regla_def                                             { $$ = RuleDeclarationSemanticAction($1); }
    | task_def                                              { $$ = TaskDeclarationSemanticAction($1); }
    | def_constante                                         { $$ = $1; }
    ;

def_constante: KW_DEFINIR IDENTIFIER COLON expresion        { $$ = ConstantDeclarationSemanticAction($2, $4); }
    ;

entity_def: KW_CLIENTE STRING OPEN_BRACE lista_atributos CLOSE_BRACE { $$ = EntitySemanticAction(CLIENTE, $2, $4); }
    | KW_PROVEEDOR STRING OPEN_BRACE lista_atributos CLOSE_BRACE     { $$ = EntitySemanticAction(PROVEEDOR, $2, $4); }
    | KW_MONOTRIBUTISTA STRING OPEN_BRACE lista_atributos CLOSE_BRACE { $$ = EntitySemanticAction(MONOTRIBUTISTA, $2, $4); }
    ;

lista_atributos: atributo                                   { $$ = $1; }
    | lista_atributos atributo                              { $$ = AttributeListSemanticAction($1, $2); }
    ;

atributo: IDENTIFIER COLON expresion                        { $$ = AttributeSemanticAction($1, $3); }
    ;

regla_def: KW_REGLA_FACTURACION STRING OPEN_BRACE lista_condiciones CLOSE_BRACE { $$ = RuleSemanticAction(FACTURACION, $2, $4); }
    | KW_REGLA_PAGO STRING OPEN_BRACE lista_condiciones CLOSE_BRACE             { $$ = RuleSemanticAction(PAGO, $2, $4); }
    | KW_REGLA_EXENCION STRING OPEN_BRACE lista_condiciones CLOSE_BRACE         { $$ = RuleSemanticAction(EXENCION, $2, $4); }
    ;

task_def: KW_SINCRONIZACION_PADRONES OPEN_BRACE lista_atributos CLOSE_BRACE { $$ = TaskSemanticAction(SINCRONIZACION, NULL, $3); }
    | KW_CIERRE_MENSUAL STRING OPEN_BRACE lista_atributos CLOSE_BRACE       { $$ = TaskSemanticAction(CIERRE, $2, $4); }
    ;

lista_condiciones: condicion                                { $$ = $1; }
    | lista_condiciones condicion                           { $$ = ConditionListSemanticAction($1, $2); }
    ;

condicion: OP_SUB condicion_logica                          { $$ = $2; }
    ;

condicion_logica: IDENTIFIER operador_relacional expresion %prec PREC_CONDICION { $$ = RelationalConditionSemanticAction($1, $2, $3); }
    | OPEN_PAREN condicion_logica CLOSE_PAREN               { $$ = $2; }
    | KW_NO condicion_logica                                { $$ = NotConditionSemanticAction($2); }
    | condicion_logica KW_Y condicion_logica %prec PREC_CONDICION { $$ = LogicalConditionSemanticAction(AND_CONDITION, $1, $3); }
    | condicion_logica KW_O condicion_logica %prec PREC_CONDICION { $$ = LogicalConditionSemanticAction(OR_CONDITION, $1, $3); }
    ;

operador_relacional: OP_LE                                  { $$ = LE; }
    | OP_GE                                                 { $$ = GE; }
    | OP_EQ                                                 { $$ = EQ; }
    ;

expresion: valor                                            { $$ = $1; }
    | expresion OP_ADD expresion                            { $$ = ExpressionSemanticAction(ADD, $1, $3); }
    | expresion OP_SUB expresion                            { $$ = ExpressionSemanticAction(SUB, $1, $3); }
    | expresion OP_MUL expresion                            { $$ = ExpressionSemanticAction(MUL, $1, $3); }
    | expresion OP_DIV expresion                            { $$ = ExpressionSemanticAction(DIV, $1, $3); }
    | OPEN_PAREN expresion CLOSE_PAREN                      { $$ = $2; }
    ;

valor: STRING                                               { $$ = StringValueSemanticAction($1); }
    | NUMBER                                                { $$ = NumberValueSemanticAction($1); }
    | KW_VERDADERO                                          { $$ = BooleanValueSemanticAction(true); }
    | KW_FALSO                                              { $$ = BooleanValueSemanticAction(false); }
    | NUMBER PERCENT                                        { $$ = PercentageValueSemanticAction($1); }
    | KW_ULTIMO_DIA_MES                                     { $$ = TimeValueSemanticAction(strdup("ultimo_dia_mes")); }
    | OPEN_BRACKET lista_valores CLOSE_BRACKET              { $$ = ListValueSemanticAction($2); }
    | IDENTIFIER                                            { $$ = IdentifierValueSemanticAction($1); }
    ;

lista_valores: valor                                        { $$ = $1; }
    | lista_valores COMMA valor                             { $$ = ValueListSemanticAction($1, $3); }
    ;

%%

