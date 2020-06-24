#include "CG.hpp"

std::string op_name[] = { "GET", "PUT", "LOAD", "STORE", "LOADI", "STOREI",
                        "ADD", "SUB", "SHIFT", "INC", "DEC", "JUMP",
                        "JPOS", "JZERO", "JNEG", "HALT" };

struct instruction code[300000];

int k = 0;
int stack_pointer = 0;

const int ACC = 0;
const int REGISTER_1 = 1;
const int REGISTER_2 = 2;
const int REGISTER_3 = 3;
const int REGISTER_4 = 4;
const int REGISTER_5 = 5; // dla petli for from //nie uzyte
const int REGISTER_6 = 6; // dla petli for to
const int REGISTER_7 = 7;
const int REGISTER_8 = 8;
const int REGISTER_9 = 9;
const int REGISTER_10 = 10;
const int REGISTER_11 = 11; //specjalnie dla znaku mod jpdl
const int DUMP = 0;

long long data_offset = 12;
long long data_location() { return data_offset++; }
long long data_releasing() { return --data_offset; }
long long data_point() { return data_offset; }

int code_offset = 0;
int reserve_loc(){
    return code_offset++;
}

int gen_label(){
    return code_offset;
}

void gen_code(enum code_ops operation, long long arg)
{
    code[code_offset].op = operation;
    code[code_offset++].arg = arg;
}


void back_patch(int addr, enum code_ops operation, long long arg){
    code[addr].op = operation;
    code[addr].arg = arg;
}

void get_value(long long val){
    bool t[100];
    long long p = 0;
    long long 
    v = val;

    
    while(v != 0) {
        if(v % 2 == 0)
            t[p] = true;
        else 
            t[p] = false;
        v = v / 2;
        p++;
    }

    gen_code(SUB, 0);

    if(val > 0) {
        gen_code(INC, 0);
        gen_code(STORE, 1);
        p--;
        while(p > 0){
            p--;
            if(t[p] == true)
                gen_code(SHIFT, 1);
            else{
                gen_code(SHIFT, 1);
                gen_code(INC, 1);
            }
        }
    } else if(val < 0){
        gen_code(INC, 0);
        gen_code(STORE, 1);
        gen_code(SUB, ACC);
        gen_code(DEC);
        p--;
        while(p > 0){
            p--;
            if(t[p] == true)
                gen_code(SHIFT, 1);
            else{
                gen_code(SHIFT, 1);
                gen_code(DEC, 1);
            }
        }
    }       
}


// USE ACC, REGISTER_1 - liczba1, REGISTER_2 - liczba2, REGISTER_3 - wynik // register 4 - znak
void gen_code_simple_multiplication(){ 
    gen_code(SUB, ACC);
    gen_code(STORE, REGISTER_4);

    gen_code(LOAD, data_releasing());
    gen_code(STORE, REGISTER_1);
    gen_code(JZERO, gen_label() + 8);
    gen_code(JPOS, gen_label() + 7);
    gen_code(LOAD, REGISTER_4);
    gen_code(INC);
    gen_code(STORE, REGISTER_4);
    gen_code(SUB, ACC);
    gen_code(SUB, REGISTER_1);
    gen_code(STORE, REGISTER_1);

    gen_code(LOAD, data_releasing());
    gen_code(STORE, REGISTER_2);
    gen_code(JZERO, gen_label() + 12);
    gen_code(JPOS, gen_label() + 11);
    gen_code(LOAD, REGISTER_4);
    gen_code(JPOS, gen_label() + 4);
    gen_code(INC);
    gen_code(STORE, REGISTER_4);
    gen_code(JUMP, gen_label() + 3);
    gen_code(DEC);
    gen_code(STORE, REGISTER_4);
    gen_code(SUB, ACC);
    gen_code(SUB, REGISTER_2);
    gen_code(STORE, REGISTER_2);
    
    gen_code(SUB, ACC);
    gen_code(STORE, REGISTER_3);
    gen_code(LOAD, REGISTER_1);
    gen_code(JZERO, gen_label() + 7);
    gen_code(DEC);
    gen_code(STORE, REGISTER_1);
    gen_code(LOAD, REGISTER_2);
    gen_code(ADD, REGISTER_3);
    gen_code(STORE, REGISTER_3);
    gen_code(JUMP, gen_label() - 7);
    
    gen_code(LOAD, REGISTER_4);
    gen_code(JZERO, gen_label() + 4);
    
    gen_code(SUB, ACC);
    gen_code(SUB, REGISTER_3);
    gen_code(JUMP, gen_label() + 2);

    gen_code(LOAD, REGISTER_3);
    gen_code(STORE, data_location());
}

// ACC, REGISTER_1, REGISTER_2, REGISTER_3
void gen_code_simple_division(){ // x1 register_1,  / x2 register_2, sign register_4
    gen_code(SUB, ACC);
    gen_code(STORE, REGISTER_4);

    gen_code(LOAD, data_releasing());
    gen_code(STORE, REGISTER_2);
    gen_code(JZERO, gen_label() + 8);
    gen_code(JPOS, gen_label() + 7);
    gen_code(LOAD, REGISTER_4);
    gen_code(INC);
    gen_code(STORE, REGISTER_4);
    gen_code(SUB, ACC);
    gen_code(SUB, REGISTER_2);
    gen_code(STORE, REGISTER_2);

    gen_code(LOAD, data_releasing());
    gen_code(STORE, REGISTER_1);
    gen_code(JZERO, gen_label() + 12);
    gen_code(JPOS, gen_label() + 11);
    gen_code(LOAD, REGISTER_4);
    gen_code(JPOS, gen_label() + 4);
    gen_code(INC);
    gen_code(STORE, REGISTER_4);
    gen_code(JUMP, gen_label() + 3);
    gen_code(DEC);
    gen_code(STORE, REGISTER_4);
    gen_code(SUB, ACC);
    gen_code(SUB, REGISTER_1);
    gen_code(STORE, REGISTER_1);

    gen_code(LOAD, REGISTER_2);
    gen_code(JZERO, gen_label() + 70);

    //register_5 miejsce poczatku elementow
    //register 6 numer aktualnego elementu
    //register 7 q
    //register 8 r
    //register 9  1
    //register 10 -1

    gen_code(LOAD, REGISTER_1);
    gen_code(STORE, REGISTER_3);
    get_value(data_location()); // uzywa registra 1
    gen_code(STORE, REGISTER_5);
    gen_code(STORE, REGISTER_6);
    gen_code(LOAD, REGISTER_3);
    gen_code(STORE, REGISTER_1);

    gen_code(SUB, ACC);
    gen_code(STORE, REGISTER_7);
    gen_code(STORE, REGISTER_8);
    gen_code(INC);
    gen_code(STORE, REGISTER_9);
    gen_code(SUB, ACC);
    gen_code(DEC);
    gen_code(STORE, REGISTER_10);
    
    gen_code(LOAD, REGISTER_1);

    gen_code(SHIFT, REGISTER_10);
    gen_code(SHIFT, REGISTER_9);
    gen_code(SUB, REGISTER_1);
    gen_code(STOREI, REGISTER_6);  //W kolejnych komorkach zapisujemy 0 albo -1 (zamiast 1)
    gen_code(LOAD, REGISTER_6);
    gen_code(INC);
    gen_code(STORE, REGISTER_6);
    gen_code(LOAD, REGISTER_1);
    gen_code(SHIFT, REGISTER_10);
    gen_code(STORE, REGISTER_1);
    gen_code(JPOS, gen_label() - 10);

    gen_code(LOAD, REGISTER_8);
    gen_code(SHIFT, REGISTER_9);
    gen_code(STORE, REGISTER_8);
    gen_code(LOAD, REGISTER_6);
    gen_code(DEC);
    gen_code(STORE, REGISTER_6);
    gen_code(LOADI, REGISTER_6);
    gen_code(STORE, REGISTER_3);
    gen_code(LOAD, REGISTER_8);
    gen_code(SUB, REGISTER_3);
    gen_code(STORE, REGISTER_8);
    gen_code(SUB, REGISTER_2);
    gen_code(JNEG, gen_label() + 7);
    gen_code(STORE, REGISTER_8);
    gen_code(LOAD, REGISTER_7);
    gen_code(SHIFT, REGISTER_9);
    gen_code(INC);
    gen_code(STORE, REGISTER_7);
    gen_code(JUMP, gen_label() + 4);
    gen_code(LOAD, REGISTER_7);
    gen_code(SHIFT, REGISTER_9);
    gen_code(STORE, REGISTER_7);
    gen_code(LOAD, REGISTER_6);
    gen_code(SUB, REGISTER_5);
    gen_code(JPOS, gen_label() - 24);

    data_releasing();
    gen_code(LOAD, REGISTER_7);
    gen_code(STORE, REGISTER_3);

    gen_code(LOAD, REGISTER_4);
    gen_code(JZERO, gen_label() + 5);
    
    gen_code(SUB, ACC);
    gen_code(SUB, REGISTER_3);
    gen_code(DEC);
    gen_code(JUMP, gen_label() + 2);
    
    gen_code(LOAD, REGISTER_3);
    gen_code(STORE, data_location());
}

void gen_code_mod(){
        gen_code(SUB, ACC);
    gen_code(STORE, REGISTER_4);
    gen_code(STORE, REGISTER_11);

    gen_code(LOAD, data_releasing());
    gen_code(STORE, REGISTER_2);
    gen_code(JZERO, gen_label() + 9);
    gen_code(JPOS, gen_label() + 8);
    gen_code(LOAD, REGISTER_4);
    gen_code(INC);
    gen_code(STORE, REGISTER_4);
    gen_code(STORE, REGISTER_11);
    gen_code(SUB, ACC);
    gen_code(SUB, REGISTER_2);
    gen_code(STORE, REGISTER_2);

    gen_code(LOAD, data_releasing());
    gen_code(STORE, REGISTER_1);
    gen_code(JZERO, gen_label() + 12);
    gen_code(JPOS, gen_label() + 11);
    gen_code(LOAD, REGISTER_4);
    gen_code(JPOS, gen_label() + 4);
    gen_code(INC);
    gen_code(STORE, REGISTER_4);
    gen_code(JUMP, gen_label() + 3);
    gen_code(DEC);
    gen_code(STORE, REGISTER_4);
    gen_code(SUB, ACC);
    gen_code(SUB, REGISTER_1);
    gen_code(STORE, REGISTER_1);

    gen_code(LOAD, REGISTER_2);
    gen_code(JZERO, gen_label() + 78);

    //register_5 miejsce poczatku elementow
    //register 6 numer aktualnego elementu
    //register 7 q
    //register 8 r
    //register 9  1
    //register 10 -1

    gen_code(LOAD, REGISTER_1);
    gen_code(STORE, REGISTER_3);
    get_value(data_location()); // uzywa registra 1
    gen_code(STORE, REGISTER_5);
    gen_code(STORE, REGISTER_6);
    gen_code(LOAD, REGISTER_3);
    gen_code(STORE, REGISTER_1);

    gen_code(SUB, ACC);
    gen_code(STORE, REGISTER_7);
    gen_code(STORE, REGISTER_8);
    gen_code(INC);
    gen_code(STORE, REGISTER_9);
    gen_code(SUB, ACC);
    gen_code(DEC);
    gen_code(STORE, REGISTER_10);
    
    gen_code(LOAD, REGISTER_1);

    gen_code(SHIFT, REGISTER_10);
    gen_code(SHIFT, REGISTER_9);
    gen_code(SUB, REGISTER_1);
    gen_code(STOREI, REGISTER_6);  //W kolejnych komorkach zapisujemy 0 albo -1 (zamiast 1)
    gen_code(LOAD, REGISTER_6);
    gen_code(INC);
    gen_code(STORE, REGISTER_6);
    gen_code(LOAD, REGISTER_1);
    gen_code(SHIFT, REGISTER_10);
    gen_code(STORE, REGISTER_1);
    gen_code(JPOS, gen_label() - 10);

    gen_code(LOAD, REGISTER_8);
    gen_code(SHIFT, REGISTER_9);
    gen_code(STORE, REGISTER_8);
    gen_code(LOAD, REGISTER_6);
    gen_code(DEC);
    gen_code(STORE, REGISTER_6);
    gen_code(LOADI, REGISTER_6);
    gen_code(STORE, REGISTER_3);
    gen_code(LOAD, REGISTER_8);
    gen_code(SUB, REGISTER_3);
    gen_code(STORE, REGISTER_8);
    gen_code(SUB, REGISTER_2);
    gen_code(JNEG, gen_label() + 7);
    gen_code(STORE, REGISTER_8);
    gen_code(LOAD, REGISTER_7);
    gen_code(SHIFT, REGISTER_9);
    gen_code(INC);
    gen_code(STORE, REGISTER_7);
    gen_code(JUMP, gen_label() + 4);
    gen_code(LOAD, REGISTER_7);
    gen_code(SHIFT, REGISTER_9);
    gen_code(STORE, REGISTER_7);
    gen_code(LOAD, REGISTER_6);
    gen_code(SUB, REGISTER_5);
    gen_code(JPOS, gen_label() - 24);

    data_releasing();
    gen_code(LOAD, REGISTER_8);
    gen_code(STORE, REGISTER_3);
    gen_code(JZERO, gen_label() + 17);

    gen_code(LOAD, REGISTER_4);
    gen_code(JZERO, gen_label() + 9);
    
    gen_code(LOAD, REGISTER_11); // + -
    gen_code(JZERO, gen_label()+ 4);
    gen_code(LOAD, REGISTER_3);
    gen_code(SUB, REGISTER_2);
    gen_code(JUMP, gen_label() + 10);

    gen_code(LOAD, REGISTER_2); // - +
    gen_code(SUB, REGISTER_3);
    gen_code(JUMP, gen_label()+ 7);
    
    gen_code(LOAD, REGISTER_11);
    gen_code(JZERO, gen_label() + 4); // - - 
    gen_code(SUB, ACC);
    gen_code(SUB, REGISTER_3);
    gen_code(JUMP, gen_label() + 2);

    gen_code(LOAD, REGISTER_3);
    gen_code(STORE, data_location());
}

void gen_code_simple_mod(){ //register 3, bool czy dzielnik jest ujemny
    gen_code(LOAD, data_releasing()); //x2
    gen_code(STORE, REGISTER_2);
    gen_code(LOAD, data_releasing()); //x1
    gen_code(STORE, REGISTER_1);

    gen_code(LOAD, REGISTER_1);
    gen_code(JNEG, gen_label() + 17);
    gen_code(JZERO, gen_label() + 31); 

    gen_code(LOAD, REGISTER_2); // 1 dodatnia
    gen_code(JPOS, gen_label() + 7);
    gen_code(JZERO, gen_label() + 28);

    gen_code(LOAD, REGISTER_1); // 1 dodatnia,2 ujemna
    gen_code(ADD, REGISTER_2);
    gen_code(JNEG, gen_label() + 25);
    gen_code(JZERO, gen_label() + 24);
    gen_code(JUMP, gen_label() - 3);

    gen_code(LOAD, REGISTER_1);  // 1, 2 dodatnia
    gen_code(SUB, REGISTER_2);
    gen_code(JNEG, gen_label() + 3);
    gen_code(STORE, REGISTER_1);
    gen_code(JUMP, gen_label() - 3);
    gen_code(LOAD, REGISTER_1);
    gen_code(JUMP, gen_label() + 16);

    gen_code(LOAD, REGISTER_2); //1 ujemna
    gen_code(JPOS, gen_label() + 9); 
    gen_code(JZERO, gen_label() + 13);

    gen_code(LOAD, REGISTER_1); //1,2 ujemna
    gen_code(SUB, REGISTER_2);
    gen_code(JPOS, gen_label() + 3);
    gen_code(STORE, REGISTER_1);
    gen_code(JUMP, gen_label() - 3);
    gen_code(LOAD, REGISTER_1);
    gen_code(JUMP, gen_label()+6);

    gen_code(LOAD, REGISTER_1); // 1 ujemna,2 dodatnia
    gen_code(ADD, REGISTER_2);
    gen_code(JPOS, gen_label() + 3);
    gen_code(JZERO, gen_label() + 2);
    gen_code(JUMP, gen_label() - 3);

    gen_code(STORE, data_location());

}

void gen_code_multiplication(){     // ACC | 1 | B | L_Wieksza | L_mniejsza
    gen_code(SUB, ACC);
    gen_code(STORE, REGISTER_4);

    gen_code(LOAD, data_releasing());
    gen_code(STORE, REGISTER_1);
    gen_code(JZERO, gen_label() + 8);
    gen_code(JPOS, gen_label() + 7);
    gen_code(LOAD, REGISTER_4);
    gen_code(INC);
    gen_code(STORE, REGISTER_4);
    gen_code(SUB, ACC);
    gen_code(SUB, REGISTER_1);
    gen_code(STORE, REGISTER_1);

    gen_code(LOAD, data_releasing());
    gen_code(STORE, REGISTER_2);
    gen_code(JZERO, gen_label() + 12);
    gen_code(JPOS, gen_label() + 11);
    gen_code(LOAD, REGISTER_4);
    gen_code(JPOS, gen_label() + 4);
    gen_code(INC);
    gen_code(STORE, REGISTER_4);
    gen_code(JUMP, gen_label() + 3);
    gen_code(DEC);
    gen_code(STORE, REGISTER_4);
    gen_code(SUB, ACC);
    gen_code(SUB, REGISTER_2);
    gen_code(STORE, REGISTER_2);
    
    gen_code(SUB, ACC);
    gen_code(STORE, REGISTER_3);
    gen_code(INC);
    gen_code(STORE, REGISTER_9);
    gen_code(SUB, ACC);
    gen_code(DEC);
    gen_code(STORE, REGISTER_10);

    gen_code(LOAD, REGISTER_1);
    gen_code(SHIFT, REGISTER_10);
    gen_code(SHIFT, REGISTER_9);
    gen_code(SUB, REGISTER_1);
    gen_code(JZERO, gen_label() + 4);
    gen_code(LOAD, REGISTER_2);
    gen_code(ADD, REGISTER_3);
    gen_code(STORE, REGISTER_3);
    gen_code(LOAD, REGISTER_2);
    gen_code(SHIFT, REGISTER_9);
    gen_code(STORE, REGISTER_2);
    gen_code(LOAD, REGISTER_1);
    gen_code(SHIFT, REGISTER_10);
    gen_code(STORE, REGISTER_1);
    gen_code(JPOS, gen_label() - 13);
    
    gen_code(LOAD, REGISTER_4);
    gen_code(JZERO, gen_label() + 4);
    
    gen_code(SUB, ACC);
    gen_code(SUB, REGISTER_3);
    gen_code(JUMP, gen_label() + 2);

    gen_code(LOAD, REGISTER_3);
    gen_code(STORE, data_location());


}