#ifndef ERROR_H
#define ERROR_H

#include <stdarg.h> /* Implementar flerror con argumentos variables */
#include <stdio.h>


void flerror(const char *fmt, ...);
void commanderror(const char *fmt, ...);
void _yyerror(const char *fmt, va_list args);

#endif