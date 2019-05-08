//
// Created by mati on 08.05.19.
//

#include <stddef.h>

extern "C" {
#include "usermem.h"
}

#include "config_target.h"


#if CONFIG_TARGET == TARGET_Linux

#else

void* malloc(size_t c){
    return usermalloc(c);
}

void free(void* p){
    return userfree(p);
}

#endif


