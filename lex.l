%{
    #include "minic_yacc.h"
    #include "string.h"
    #include "ast.h"
    int yycolumn=1;
    #define YY_USER_ACTION yylloc.first_line=yylloc.last_line=yylineno; \
    yylloc.first_column = yycolumn;	yylloc.last_column = yycolumn+yyleng-1; yycolumn += yyleng;//每次匹配之前执行的动作，yylloc表示当前词法单元所对应的位置信息
    typedef union 
    {
        int    	int_val;
        float  	float_val;
        char 	char_val;
        char   	id[32];
        struct 	node *node;
    } YYLVAL;
    #define YYSTYPE YYLVAL
    void comment();
%}
%option yylineno

%%
[0-9]+        {yylval.int_val=atoi(yytext); return INT;}
([0-9]*\.[0-9]+)|([0-9]+\.)     {yylval.float_val=atof(yytext); return FLOAT;}
\'.\'|\'\\.\' {yylval.char_val=yytext[1]; return CHAR;}
"int"        {strcpy(yylval.id, yytext);return KEYINT;}
"void"       {strcpy(yylval.id, yytext);return KEYVOID;}
"float"      {strcpy(yylval.id, yytext);return KEYFLOAT;}
"char"       {strcpy(yylval.id, yytext);return KEYCHAR;}

">"         {return GT;}
"<"         {return LT;}
">="        {return GE;}
"<="        {return LE;}
"=="        {return EQUAL;}
"!="        {return NE;}


"if"        {return IF;}
"else"      {return ELSE;}
"while"     {return WHILE;}
"return"    {return RETURN;}
"break"     {return BREAK;}
"continue"  {return CONTINUE;}

[A-Za-z][A-Za-z0-9]*      {strcpy(yylval.id,  yytext); return ID;}

"*"         {return MUL;}
"/"         {return DIV;}
"%"         {return MOD;}
"+"         {return PLUS;}
"-"         {return MINUS;}
"="         {return ASSIGNOP;}

"&&"        {return AND;}
"||"        {return OR;}

"("			{return LP;}
")"			{return RP;}
"["         {return LBS;}
"]"         {return RBS;}
"{"			{return LB;}
"}"			{return RB;}

","         {return COMMA;}
";"			{return SEMI;}


"//"[^\n]*\n  ;
"/*"        comment();

[\n]        {yycolumn=1;}   
[ \r\t]     {}   
.			{
                printf("Error Occurs at Line %d: Unknown characters \'%s\'\n", yylineno, yytext);
            }
%%
int yywrap()
{
	return 1;
}

void comment()
{
    char c, c1;
loop:
    while ((c = input()) != '*' && c != 0)
        putchar(c);

    if ((c1 = input()) != '/' && c != 0)
    {
        unput(c1);
        goto loop;
    }
}
