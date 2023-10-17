#pragma once

#include <stdlib.h>

// This function and a define below are workaround for bug EGD-7727.
// They can be removed when bug is fixed
inline void* khashOwnRealloc(void* ptr, size_t size) {
    if (ptr) {
        return realloc (ptr, size);
    }
    else {
        return malloc (size);
    }
}

#define krealloc(P,Z) khashOwnRealloc(P,Z)

#include <khash.h>
