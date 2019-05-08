//
// Created by mati on 08.05.19.
//

#include <stddef.h>

extern "C" {
#include "../../source/memory/usermem.h"
}



void* malloc(size_t c){
    return usermalloc(c);
}

void free(void* p){
    return userfree(p);
}



