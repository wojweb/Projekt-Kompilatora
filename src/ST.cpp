#include "ST.hpp"
#include "CG.hpp"

using namespace std;

symrec *sym_table = nullptr;



symrec* putsym(string sym_name, long long start, long long end, bool modifiable){
    symrec *ptr = new symrec;
    ptr->name = sym_name;
    ptr->next = sym_table;
    ptr->start = start;
    ptr->end = end;
    ptr->offset = data_location();
    ptr->modifiable = modifiable;
    for(long long i = start; i < end; i++)
        data_location();
    sym_table = ptr;
    return ptr;
}

symrec* getsym(string sym_name){
    symrec *ptr;
    for(ptr = sym_table; ptr != nullptr; ptr = ptr->next)
        if(ptr->name == sym_name)
            return ptr;
    
    return nullptr;
}

void remsym(string sym_name){
    symrec *ptr;
    if(sym_table->name == sym_name){
        ptr = sym_table->next;
        delete sym_table;
        sym_table = ptr;
        data_releasing();
    } else{
        for(ptr = sym_table; ptr->next != nullptr; ptr = ptr->next)
            if(ptr->next->name == sym_name){
                symrec *temp;
                temp = ptr->next->next;
                delete ptr->next;
                ptr->next = temp;
                data_releasing();
                break;
            }
    }
}