#!/bin/bash
bison -d parser.y
flex lexer.l
g++ ST.cpp CG.cpp parser.tab.c lex.yy.c 
