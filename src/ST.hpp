#include <string>

struct symrec
{
    std::string name;
    long long offset;
    long long start;
    long long end;
    bool modifiable;
    struct symrec *next;

};

typedef struct symrec symrec;

// symrec *identifier;
extern symrec *sym_table;
symrec *putsym(std::string sym_name, long long start = 0, long long end = 0, bool modifiable = true);
symrec *getsym(std::string sym_name);
void remsym(std::string sym_name);

