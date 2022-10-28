#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED
#include "Block.h"
#include "Transaction.h"
#include "main.h"
#include <string>
#include <cstdlib>
#include <iostream>
#include <cstring>

using namespace std;

struct command_t
{
 int has_arg;
 const char *name;
 void (*parse)(string &); // Puntero a funcion de opciones
};

class Command
{
private:
    command_t * command_table;

public:
    Command(command_t *);
    
    void parse(istream *is);
    
};

#endif