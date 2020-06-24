
all: compiler

compiler: parser.tab.c lex.yy.c 
	g++ ST.cpp CG.cpp parser.tab.c lex.yy.c -o compiler



lex.yy.c: lexer.l
	flex lexer.l 

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

