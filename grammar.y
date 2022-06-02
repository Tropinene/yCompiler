%error-verbose
%locations
%{
    #include "stdio.h"
    #include "math.h"
    #include "string.h"
    #include "ast.h"
    #include <stdarg.h>
    extern int yylineno;
    extern char *yytext;
    extern FILE *yyin;
    void yyerror(const char* fmt, ...);
    void createAST(struct node *, int);
    int TypeCheck(char* type);
    int yylex();
%}

%union {
	int    	int_val;
	float  	float_val;
    char 	char_val;
	char   	id[32];
	struct 	node *node;
};

// 定义非终结符
%type <node>    program definitions definition func_declare statements statement local_var_expr local_var_id 
%type <node>    v_params v_param params param 
%type <node>    local_var_def global_var_def func_def func_call
%type <node>    global_var_expr global_var_id type expr func_name param_name  v_param_name

// 定义终结符
%token <id>         ID KEYINT KEYFLOAT KEYCHAR KEYVOID
%token <int_val>    INT
%token <float_val>  FLOAT
%token <char_val>   CHAR
%token SEMI COMMA LP RP LB RB LBS RBS 
%token ASSIGNOP RETURN MUL DIV PLUS MINUS IF MOD IF_THEN_ELSE WHILE AND OR GT LT GE LE NE EQUAL BREAK CONTINUE

%left ASSIGNOP
%left OR
%left AND
%left GT LT GE LE NE EQUAL
// %left SELFPLUS SLEFMINUS
// %left MINUSASS PLUSASS
%left PLUS MINUS
%left MUL DIV
// %right UMINUS NOT 

%nonassoc LOWER_THEN_ELSE
%nonassoc ELSE

%%

program		    : 	definitions {
                    startSemanticAnalysis($1);
                }
                ;

definitions     :   {
                    $$ = NULL;
                } 
                |   definition definitions {
                    $$ = makeNode(DEF, $1, $2, NULL, yylineno);
                }
                ;

definition	    :	global_var_def {
                    $$ = $1;
                }
                |   func_def {
                    $$ = $1;
                }
                ;

global_var_def  :   type global_var_expr SEMI {
                    $$ = makeNode(G_V_DEF, $1, $2, NULL, yylineno);
                }
                ;

global_var_expr :   global_var_id {
                    $$ = makeNode(G_V_EXPR, $1, NULL, NULL, yylineno);
                }
                |   global_var_id ASSIGNOP expr {
                    $$ = makeNode(G_ASSIGNOP, $1, $3, NULL, yylineno);
                    strcpy($$->type_id, "ASSIGNOP");
                }
                ;

global_var_id   :   ID {
                    $$ = makeNode(G_ID, $1, NULL, NULL, yylineno);
				    strcpy($$->type_id, $1);
                }
                ;

func_def        :   type func_declare SEMI {
                    $$ = makeNode(F_DEF, $1, $2, NULL, yylineno);
                }
                |   type func_declare LB statements RB {
                    $$ = makeNode(F_DEF, $1, $2, $4, yylineno);
                }
                ;

func_declare    :   func_name LP params RP {
                    $$ = makeNode(F_DEC, $1, $3, NULL, yylineno);
                }
                ;

func_name       :   ID {
                    $$ = makeNode(F_NAME, $1, NULL, NULL, yylineno);
                    strcpy($$->type_id, $1);
                }

params          :   {
                    $$ = NULL;
                }
                |   param {
                    $$ = $1;
                }
                |   param COMMA params {
                    $$ = makeNode(PARAMS, $1, $3, NULL, yylineno);
                }
                ;

param           :   type param_name {
                    $$ = makeNode(PARAM, $1, $2, NULL, yylineno);
                }
                ;

param_name      :   ID {
                    $$ = makeNode(P_NAME, $1, NULL, NULL, yylineno);
                    strcpy($$->type_id, $1);
                }

type            :   KEYINT {
                    $$ = makeNode(KEYINT, NULL, NULL, NULL, yylineno);
                    strcpy($$->type_id, "int");
                    // $$->type = TypeCheck($1);
                }
                |   KEYVOID {
                    $$ = makeNode(KEYVOID, NULL, NULL, NULL, yylineno);
                    strcpy($$->type_id, "void");
                } 
                |   KEYFLOAT {
                    $$ = makeNode(KEYFLOAT, NULL, NULL, NULL, yylineno);
                    strcpy($$->type_id, "float");
                    // $$->type = TypeCheck($1);
                } 
                |   KEYCHAR {
                    $$ = makeNode(KEYFLOAT, NULL, NULL, NULL, yylineno);
                    strcpy($$->type_id, "char");
                }
                ;

statements      : 	{
                    $$ = NULL; 
                }  
                | 	statement statements {
                    $$ = makeNode(STMTS, $1, $2, NULL, yylineno);
                }
                ;

statement       :   expr SEMI {
                    $$ = makeNode(EXPR, $1, NULL, NULL, yylineno);
                }
                | 	local_var_def statements {
                    $$ = makeNode(STMT, $1, $2, NULL, yylineno);
                }
                |   IF LP expr RP LB statements RB %prec LOWER_THEN_ELSE {  // only-if
                    $$ = makeNode(IF_THEN, $3, $6, NULL, yylineno);
                }
                | 	IF LP expr RP LB statements RB ELSE LB statements RB {	//if-else 语句
                    $$ = makeNode(IF_THEN_ELSE, $3, $6, $10, yylineno);
                }
                | 	WHILE LP expr RP LB statements RB {
                    $$ = makeNode(WHILE, $3, $6, NULL, yylineno);
                }
                | 	RETURN expr SEMI {
                    $$ = makeNode(RETURN, $2, NULL, NULL, yylineno);
                }
                |   BREAK SEMI {
                    $$ = makeNode(BREAK, NULL, NULL, NULL, yylineno);
                }
                ;

local_var_def   :   type local_var_expr SEMI {
                    $$ = makeNode(L_V_DEF, $1, $2, NULL, yylineno);
                }
                ;

local_var_expr  :   local_var_id {
                    $$ = makeNode(L_V_EXPR, $1, NULL, NULL, yylineno);
                }
                |   local_var_id ASSIGNOP expr {
                    $$ = makeNode(L_ASSIGNOP, $1, $3, NULL, yylineno);
                    strcpy($$->type_id, "ASSIGNOP");
                }
                ;

local_var_id    :   ID {
                    $$ = makeNode(L_ID, $1, NULL, NULL, yylineno);
				    strcpy($$->type_id, $1);
                }
                ;

expr            :   expr ASSIGNOP expr {
                    $$ = makeNode(ASSIGNOP, $1, $3, NULL, yylineno);
                    strcpy($$->type_id, "ASSIGNOP");
                }
                |   expr GT expr {
                    $$ = makeNode(GT,$1,$3,NULL,yylineno);
                    strcpy($$->type_id, ">");
                }
                |   expr LT expr {
                    $$ = makeNode(LT,$1,$3,NULL,yylineno);
                    strcpy($$->type_id, "<");
                }
                |   expr GE expr {
                    $$ = makeNode(GE,$1,$3,NULL,yylineno);
                    strcpy($$->type_id, ">=");
                }
                |   expr LE expr {
                    $$ = makeNode(LE,$1,$3,NULL,yylineno);
                    strcpy($$->type_id, "<=");
                }
                |   expr EQUAL expr {
                    $$ = makeNode(EQUAL,$1,$3,NULL,yylineno);
                    strcpy($$->type_id, "==");
                }
                |   expr NE expr {
                    $$ = makeNode(NE,$1,$3,NULL,yylineno);
                    strcpy($$->type_id, "!=");
                }
                | 	expr PLUS expr  {
                    $$ = makeNode(PLUS, $1, $3, NULL, yylineno);
                    strcpy($$->type_id, "PLUS");
                }
                | 	expr MINUS expr {
                    $$ = makeNode(MINUS, $1, $3, NULL, yylineno);
                    strcpy($$->type_id, "MINUS");
                }
                | 	expr MUL expr  {
                    $$ = makeNode(MUL, $1, $3, NULL, yylineno);
                    strcpy($$->type_id, "MUL");
                }
                | 	expr DIV expr   {
                    $$ = makeNode(DIV, $1, $3, NULL, yylineno);
                    strcpy($$->type_id, "DIV");
                }
                |   expr MOD expr {
                    $$ = makeNode(DIV, $1, $3, NULL, yylineno);
                    strcpy($$->type_id, "MOD");
                }
                |   expr AND expr {
                    $$ = makeNode(AND, $1, $3, NULL, yylineno);
                    strcpy($$->type_id, "AND");
                }
                |   expr OR expr {
                    $$ = makeNode(OR, $1, $3, NULL, yylineno);
                    strcpy($$->type_id, "OR");
                }
                |   func_call LP v_params RP {
                    $$ = makeNode(FUNC_CALL, $1, $3, NULL, yylineno);
                }
                |   ID {
                    $$ = makeNode(ID, NULL, NULL, NULL, yylineno);
				    strcpy($$->type_id, $1);
                }
                |   INT {
                    $$ = makeNode(INT, NULL, NULL, NULL, yylineno);
                    $$->type_int = $1;
                    $$->type = INT;
                }
                ; 

func_call       :   func_name {
                    $$ = makeNode(F_C_NAME, NULL, NULL, NULL, yylineno);
                    strcpy($$->type_id, $1->type_id);
                }
                ;

v_params        :   {
                    $$ = NULL;
                }
                |   v_param {
                    $$ = $1;
                }
                |   v_param COMMA v_params {
                    $$ = makeNode(V_PARAMS, $1, $3, NULL, yylineno);
                }
                ;

v_param         :   v_param_name {
                    $$ = makeNode(V_PARAM, $1, NULL, NULL, yylineno);
                }
                ;

v_param_name    :   ID {
                    $$ = makeNode(V_P_NAME, $1, NULL, NULL, yylineno);
                    strcpy($$->type_id, $1);
                }
                |   INT {
                    $$ = makeNode(INT, NULL, NULL, NULL, yylineno);
                    $$->type_int = $1;
                    $$->type = INT;
                }
                ;  

%%
void yyerror(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "Grammar Error at Line %d Column %d: ", yylloc.first_line,yylloc.first_column);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, ".\n");
}