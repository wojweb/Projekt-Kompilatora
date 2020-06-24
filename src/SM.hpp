#include <string>

enum code_ops { GET, PUT, LOAD, STORE, LOADI, STOREI,
                ADD, SUB, SHIFT, INC, DEC, JUMP,
                JPOS, JZERO, JNEG, HALT };

std::string op_name[] = {    "GET", "PUT", "LOAD", "STORE", "LOADI", "STOREI",
                        "ADD", "SUB", "SHIFT", "INC", "DEC", "JUMP",
                        "JPOS", "JZERO", "JNEG", "HALT" };

