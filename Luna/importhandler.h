#ifndef IMPORT_HANDLER_H
#define IMPORT_HANDLER_H


#include "TS.h"
#include "list.h"

// Initialize import handler
void import_init(void);

// Load a library and add to tracked imports
int import_library(const char* libname);

// Try to resolve a symbol from all loaded libraries
symrec import_resolve_symbol(char* symname);

// Cleanup loaded libraries
void import_cleanup(void);

#endif