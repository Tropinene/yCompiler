all:
	make clean
	make prepare
	make minic

prepare: lex.l grammar.y
	flex -o minic_lex.c --header-file=minic_lex.h lex.l
	bison -d -o minic_yacc.c grammar.y
	
minic:minic_lex.c minic_yacc.c ast.c genCode.c main.c 
	gcc -o minic minic_lex.c minic_yacc.c ast.c genCode.c main.c
	
clean:
	rm -rf *.o
	rm -rf *.s
	rm -rf *.asm
	rm -rf *.ir
	rm -rf *.dSYM
	rm -f minic_lex.c minic_lex.h
	rm -f minic_yacc.c minic_yacc.h
	rm -f minic 
	rm -f tree.gv
