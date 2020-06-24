#include <string>

enum code_ops { GET, PUT, LOAD, STORE, LOADI, STOREI,
                ADD, SUB, SHIFT, INC, DEC, JUMP,
                JPOS, JZERO, JNEG, HALT };



struct instruction {
    enum code_ops op;
    long long arg;
};

extern std::string op_name[];
extern struct instruction code[];
extern int code_offset;
extern int stack[];
extern int k;
extern const int ACC;
extern const int REGISTER_1;
extern const int REGISTER_2;
extern const int REGISTER_3;
extern const int REGISTER_4;
extern const int DUMP;


// extern int data_offset;

long long data_location();
long long data_releasing();
long long data_point();
int reserve_loc();
int gen_label();
void gen_code(enum code_ops op, long long arg = DUMP);
void back_patch(int addr, enum code_ops op, long long arg);
void get_value(long long val);
void gen_code_multiplication();
void gen_code_simple_multiplication();
void gen_code_simple_division();
void gen_code_simple_mod();
void gen_code_mod();