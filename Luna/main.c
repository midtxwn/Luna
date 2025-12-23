
#include "TS.h"
#include <stdio.h>
#include <signal.h>
#include "COMMANDS.h"

extern int yyparse(void);

list_t* OPEN_FILES_FILENAMES;
TablaSimbolos* tabla;
//extern int yydebug;
//extern int yy_flex_debug;
void signal_handler(int signum) {
    (void)signum; // Evita advertencias sobre el parámetro no usado
    cmd_quit(list_create()); // Llama a cmd_quit
}

int main(void) {
    signal(SIGINT, signal_handler); // Usa el wrapper
    
    tabla = TS_crear();
    OPEN_FILES_FILENAMES = list_create(); //usada en el analizador lexico para evitar bucles infinitos en a la hora de cargar archivos (LOAD)
    //yydebug = 0;
    //yy_flex_debug = 1;
    printf(">>");
    if (yyparse() != 0);
    TS_destruir(tabla);
    list_destroy(OPEN_FILES_FILENAMES);
    return 0;
}