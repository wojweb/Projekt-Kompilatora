%{

#include "parser.tab.h"
#include "string.h"

%}

%option noyywrap


DIGIT   [0-9]
ID      [_a-z]+


%%

DECLARE     return(DECLARE);
"("           return('(');
")"           return(')');
":"           return(':');

BEGIN       return(MBEGIN);
END         return(END);
ASSIGN      return(ASSIGN);
READ        return(READ);
WRITE       return(WRITE);
IF          return(IF);
THEN        return(THEN);
ELSE        return(ELSE);
ENDIF       return(ENDIF);
WHILE       return(WHILE);
DO          { return(DO); }
ENDWHILE    return(ENDWHILE);
ENDDO       return(ENDDO);
FOR         { return(FOR);}
FROM        return(FROM);
TO          return(TO);
DOWNTO      return(DOWNTO);
ENDFOR      return(ENDFOR);
PLUS        return('+');
MINUS       return('-');
TIMES       return('*');
DIV         return('/');
MOD         return('%');
EQ          return(EQ);
NEQ         return(NEQ);
LE          return(LE);
GE          return(GE);
LEQ         return(LEQ);
GEQ         return(GEQ);
{ID}        { yylval.id  = (char*) strdup(yytext); return(PIDENTIFIER); } 
-?{DIGIT}+    { yylval.intval = atoi(yytext); return(NUM);}
;           return(';');
,           return(',');
\[[^\]]*\]
\n
" "
\t

%%