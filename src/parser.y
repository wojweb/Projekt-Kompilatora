%{

#include<iostream>
#include<fstream>
#include<string>
#include "ST.hpp"
#include "CG.hpp"

using namespace std;

// Prototypes to keep the compiler happy
void yyerror (const char *error);
int yylex(); 
extern FILE *yyin;
int errors;

struct lbs
{
    int for_goto;
    int for_jump_false;
};

struct ident
{
    char* id;
    long long pos;
    char* second;
};



void install(string sym_name, long long start = 0, long long end = 0, bool modifiable = true){
    symrec *s = getsym(sym_name);
    if(start > end){
        cout << sym_name << " niewlasciwe indeksy tablicy start-" << start << " end-" << end;
        exit(0);
    }
    if(s == nullptr || modifiable == false)
        s = putsym(sym_name, start, end, modifiable);
    else {
        // errors++;
        cout << sym_name << " jest juz zadeklarowany\n";
        exit(0);
    }
}

void context_check(enum code_ops operation, char* sym_name, long long pos = 0, char* second_name = nullptr){
    symrec* identifier = getsym(sym_name);
    if(identifier == nullptr){
        cout << sym_name << " jest niezadeklarowany\n";
        exit(0);
    }
    if(second_name == nullptr){
        if((pos < identifier->start || pos > identifier->end) && identifier->modifiable)
            cout << sym_name << "(" << pos << ") jest poza zakresem\n";
        else {
            if(!identifier->modifiable && (operation == STORE || operation == STOREI)){
                cout << "Nie mozna modyfikowac zmiennych lokalnych\n";
                exit(0);
            }
            gen_code(operation, identifier->offset + pos - identifier->start);

        }
    } else {
        symrec* second_identifier = getsym(second_name);
        if(second_identifier == nullptr){
        cout << second_name << " jest niezadeklarowany\n";
        exit(0);
        }
        gen_code(STORE, REGISTER_2);
        gen_code(SUB, ACC);
        // for(int i = 0; i < identifier->offset - identifier->start; i++){
        //     gen_code(INC);
        // }

        get_value(identifier->offset - identifier->start);        

        gen_code(ADD, second_identifier->offset);
        gen_code(STORE, REGISTER_1);
        gen_code(LOAD, REGISTER_2);
        gen_code(operation, REGISTER_1);
        }
}

%}
%union {
    char*           id;
    long long             intval;
    struct  lbs*    lbls;
    struct ident*   idents;
}

%start program
%type <lbls> endif
%type <idents> identifier
%token DECLARE MBEGIN END
%token FROM TO ENDFOR DOWNTO
%token <lbls> IF WHILE DO FOR
%token ENDWHILE ENDDO
%token THEN ELSE ENDIF
%token READ WRITE
%token ASSIGN
%token <intval> NUM
%token <id> PIDENTIFIER
%token EQ NEQ LE GE LEQ GEQ
%left '-' '+' '*'

%%

program : DECLARE declarations MBEGIN commands END { gen_code(HALT, 1); }
;

declarations : declarations ',' PIDENTIFIER { install($3); }
    | declarations ',' PIDENTIFIER '(' NUM ':' NUM ')' { install($3, $5, $7); }
    | PIDENTIFIER { install($1); }
    | PIDENTIFIER '(' NUM ':' NUM ')' { install($1, $3, $5); }
;

commands: command
    | commands command
;
command : identifier ASSIGN expression ';' {
                        gen_code(LOAD, data_releasing());
                        if($1->second == nullptr){
                            context_check(STORE, $1->id, $1->pos);
                        } else {
                            context_check(STOREI, $1->id, 0, $1->second);
                        }
    }
    | IF condition   {
                        gen_code(LOAD, data_releasing());
                        $1 = new lbs;
                        $1->for_jump_false = reserve_loc();
    }
    THEN commands endif { 

                        if($6 != nullptr){
                            back_patch($1->for_jump_false, JZERO, $6->for_jump_false);
                            back_patch($6->for_goto, JUMP, gen_label());
                        } else
                            back_patch($1->for_jump_false, JZERO, gen_label());

    }
    | WHILE {
                        $1 = new lbs;
                        $1->for_goto = gen_label();
    }
    condition {
                        gen_code(LOAD, data_releasing());
                        $1->for_jump_false = reserve_loc();

    }
    DO commands ENDWHILE {
                        gen_code(JUMP, $1->for_goto);
                        back_patch($1->for_jump_false, JZERO, gen_label());
    }
    | DO {
                        $1 = new lbs;
                        $1->for_goto = gen_label();
                        
    } commands WHILE condition ENDDO {
                        gen_code(LOAD, data_releasing());
                        gen_code(JPOS, $1->for_goto);
    }
    | FOR PIDENTIFIER FROM value TO value DO {
                        $1 = new lbs;                       
                        install($2, 0, 0, false);
                        gen_code(LOAD, data_point() - 3);
                        gen_code(STORE, data_point() - 1);
                        $1->for_goto = gen_label();
                        gen_code(LOAD, data_point() - 1);
                        gen_code(SUB, data_point() - 2);
                        $1->for_jump_false = reserve_loc();
    } commands ENDFOR {
                        gen_code(LOAD, data_point() - 1);
                        gen_code(INC);
                        gen_code(STORE, data_point() - 1);
                        gen_code(JUMP, $1->for_goto);
                        back_patch($1->for_jump_false, JPOS, gen_label());
                        data_releasing();
                        data_releasing();
                        
                        remsym($2);
    } 
    | FOR PIDENTIFIER FROM value DOWNTO value DO
    {
                        $1 = new lbs;                       
                        install($2, 0, 0, false);
                        gen_code(LOAD, data_point() - 3);
                        gen_code(STORE, data_point() - 1);
                        $1->for_goto = gen_label();
                        gen_code(LOAD, data_point() - 1);
                        gen_code(SUB, data_point() - 2);
                        $1->for_jump_false = reserve_loc();
    } commands ENDFOR {
                        gen_code(LOAD, data_point() - 1);
                        gen_code(DEC);
                        gen_code(STORE, data_point() - 1);
                        gen_code(JUMP, $1->for_goto);
                        back_patch($1->for_jump_false, JNEG, gen_label());
                        data_releasing();
                        data_releasing();
                        
                        remsym($2);
    }
    | READ identifier ';' {
                        gen_code(GET, 0);
                        if($2->second == nullptr){
                            context_check(STORE, $2->id, $2->pos);
                        } else {
                            context_check(STOREI, $2->id, 0, $2->second);
                        }
    }
    | WRITE value ';' {
                        gen_code(LOAD, data_releasing());
                        gen_code(PUT, 0);
    }  
;

endif :  ENDIF { 
                        $$ = nullptr;
    }
    | ELSE {
                        $<lbls>$ = new lbs;
                        $<lbls>$->for_goto = reserve_loc();
                        $<lbls>$->for_jump_false = gen_label();
    }
    commands ENDIF { $$ = $<lbls>2; }
;

expression : value 
    | value '+' value {
                        gen_code(LOAD, data_releasing());
                        gen_code(ADD, data_releasing());
                        gen_code(STORE, data_location());
                    }
    | value '-' value {
                        gen_code(LOAD, data_releasing());
                        gen_code(STORE, REGISTER_1);
                        gen_code(LOAD, data_releasing());
                        gen_code(SUB, REGISTER_1);
                        gen_code(STORE, data_location());
                    }
    | value '*' value {
                        gen_code_multiplication();
    }
    | value '/' value {
                        gen_code_simple_division();
    }
    | value '%' value {
                        gen_code_mod();
    }
;

condition : value EQ value {
                        gen_code(LOAD, data_releasing());
                        gen_code(SUB, data_releasing());
                        gen_code(JPOS, gen_label() + 4);                        
                        gen_code(JNEG, gen_label() + 3);
                        gen_code(INC, DUMP);
                        gen_code(JUMP, gen_label() + 2);
                        gen_code(SUB, ACC);
                        gen_code(STORE, data_location());
}
    | value NEQ value {
                        gen_code(LOAD, data_releasing());
                        gen_code(SUB, data_releasing());
                        gen_code(JZERO, gen_label() + 3);
                        gen_code(SUB, ACC);
                        gen_code(INC, DUMP);
                        gen_code(STORE, data_location());
    }
    | value LE value {
                        gen_code(LOAD, data_releasing());
                        gen_code(SUB, data_releasing());
                        gen_code(JNEG, gen_label() + 5);
                        gen_code(JZERO, gen_label() + 4);
                        gen_code(SUB, ACC);
                        gen_code(INC, DUMP);
                        gen_code(JUMP, gen_label() + 2);
                        gen_code(SUB, ACC);
                        gen_code(STORE, data_location());
    }
    | value GE value {
                        gen_code(LOAD, data_releasing());
                        gen_code(SUB, data_releasing());
                        gen_code(JPOS, gen_label() + 5);
                        gen_code(JZERO, gen_label() + 4);
                        gen_code(SUB, ACC);
                        gen_code(INC, DUMP);
                        gen_code(JUMP, gen_label() + 2);
                        gen_code(SUB, ACC);
                        gen_code(STORE, data_location());
    }
    | value LEQ value {
                        gen_code(LOAD, data_releasing());
                        gen_code(SUB, data_releasing());
                        gen_code(JNEG, gen_label() + 4);
                        gen_code(SUB, ACC);
                        gen_code(INC, DUMP);
                        gen_code(JUMP, gen_label() + 2);
                        gen_code(SUB, ACC);
                        gen_code(STORE, data_location());
                        
    }
    | value GEQ value { 
                        gen_code(LOAD, data_releasing());
                        gen_code(SUB, data_releasing());
                        gen_code(JPOS, gen_label() + 4);
                        gen_code(SUB, ACC);
                        gen_code(INC, DUMP);
                        gen_code(JUMP, gen_label() + 2);
                        gen_code(SUB, ACC);
                        gen_code(STORE, data_location());
                        
    }
;
value : NUM  {
                        get_value($1);
                        gen_code(STORE, data_location());
                    } 
    | identifier { 
                        if($1->second == nullptr){
                            context_check(LOAD, $1->id, $1->pos);
                        } else {
                            context_check(LOADI, $1->id, 0, $1->second);
                        }
                        gen_code(STORE, data_location());

                    }
;

identifier : PIDENTIFIER {
                        $$ = new ident;
                        $$->id = $1;
                        $$->pos = 0;
                        $$->second = nullptr;
}
    | PIDENTIFIER '(' PIDENTIFIER ')' {
                        $$ = new ident;
                        $$->id = $1;
                        $$->pos = 0;
                        $$->second = $3;
    }
    | PIDENTIFIER '(' NUM ')' {
                        $$ = new ident;
                        $$->id = $1;
                        $$->pos = $3;
                        $$->second = nullptr;
    }
;

%%

void yyerror(const char *s) {
  cout << "EEK, parse error!  Message: " << s << endl;
  // might as well halt now:
  exit(-1);
}


int main(int, char** args){
    FILE *myfile = fopen(args[1], "r");
    if(!myfile){
        cout << "Nie moge  otworzyc pliku" << endl;
        return -1;
    }
    yyin = myfile;

    yyparse();

    ofstream os;
    os.open(args[2]);
    

    for(int i = 0; i < code_offset; i++){
        switch(code[i].op){
            case GET:
            case PUT:
            case INC:
            case DEC:
            case HALT:
            // cout << i << " " << op_name[code[i].op] << endl;
            os << op_name[code[i].op] << endl;
            break;
            
            default:
            // cout << i << " " << op_name[code[i].op] << " " << code[i].arg << endl;
            os << op_name[code[i].op] << " " << code[i].arg << endl;
            break;
        }
    }

    
}
    