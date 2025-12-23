// COMMONDEF.h
#ifndef COMMONDEF_H
#define COMMONDEF_H

#include "list.h"
#include <math.h>

typedef double (func_t) (double);  // Tipo para funciones (ej: sin, cos)
typedef void (command_t)(list_t*); // Tipo para funciones (ej: sin, cos)

typedef struct {
    char* name;
    int type;           // VAR o FUN
    union {
        double var;     // Valor si es VAR
        func_t *fun;    // Puntero a función si es FUN
        command_t *cmd;
    } value;
} symrec;

#endif