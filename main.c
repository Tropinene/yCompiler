#include "stdio.h"
#include "math.h"
#include "string.h"
#include "ast.h"
#include "genCode.h"


extern FILE *yyin;
extern int yylineno;

int main(int argc, char *argv[])
{
    if(argc == 3 && argv[1][1] == 'a') {    // build ast tree
        OPTION = argv[1][1];
        yyin = fopen(argv[2],"r");
    }
    else if(argc == 3 && argv[1][1] == 'i') {   // generate interface code
        OPTION = argv[1][1];
        yyin = fopen(argv[2], "r");
    }
    else { 
        printf("Error: Invalid arguments\n");
        return 0;
    }
	if (!yyin) {
        return 0;
    }
	yylineno=1;
	yyparse();

	return 0;
}