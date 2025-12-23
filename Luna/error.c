#include "error.h"


void flerror(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr,"LEXICAL ERROR:");
    vfprintf(stderr, fmt, args);
    fprintf(stderr,"\n>>");
    va_end(args);
}

void commanderror(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr,"BAD COMMAND USAGE: ERROR:");
    vfprintf(stderr, fmt, args);
    fprintf(stderr,"\n>>");
    va_end(args);
}


void _yyerror(const char *fmt, va_list args) {
    fprintf(stderr, "SYNTAX ERROR:");
    vfprintf(stderr, fmt, args); // Imprime el mensaje de error
    fprintf(stderr,"\n>>");
}

