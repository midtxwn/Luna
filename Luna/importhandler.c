#include "importhandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "COMMONDEF.h"
#include <dlfcn.h>
#include "parser.tab.h"
#include "error.h"

static list_t* loaded_libs = NULL;

void import_init(void) {
    if (!loaded_libs) {
        loaded_libs = list_create();
    }
}

int import_library(const char* libname) {
    if (!loaded_libs) return 0;

    void* handle = dlopen(libname, RTLD_LAZY);
    if (!handle) {
        commanderror("IMPORT error: %s\n", dlerror());
        return 0;
    }

    list_append(loaded_libs, handle);
    return 1;
}

symrec import_resolve_symbol(char* symname) {
    symrec new_sym;
    new_sym.type = VAR;
    if (!loaded_libs || list_get_length(loaded_libs) == 0) 
    {
        return new_sym;
    }
        

    list_reset_iterator(loaded_libs);
    while (list_has_next(loaded_libs)) {
        void* handle = list_next(loaded_libs);
        void* sym = dlsym(handle, symname);
        
        if (sym) {
            // Create and populate symbol record
            
            new_sym.name = symname;
            new_sym.type = FUN;
            new_sym.value.fun = (double (*)(double))sym;
            return new_sym;
        }
    }
    return new_sym;
}

void import_cleanup(void) {
    if (loaded_libs) {
        list_reset_iterator(loaded_libs);
        while (list_has_next(loaded_libs)) {
            void* handle = list_next(loaded_libs);
            dlclose(handle);
        }
        list_destroy(loaded_libs);
        loaded_libs = NULL;
    }
}