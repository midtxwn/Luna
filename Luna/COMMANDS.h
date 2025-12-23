#ifndef COMMANDS_H
#define COMMANDS_H

#include "list.h"
#include "TS.h"  // Para TablaSimbolos y funciones relacionadas

// Puntero externo a la tabla de símbolos
extern TablaSimbolos* tabla;

// Declaraciones de funciones para comandos
void cmd_load(list_t* params);
void cmd_echo(list_t* params);
void cmd_print(list_t* params);
void cmd_quit(list_t* params);
void cmd_workspace(list_t* params);
void cmd_clear(list_t* params);
void cmd_clean(list_t* params);
void cmd_help(list_t* params);
void cmd_import(list_t* params);

#endif // COMMANDS_H