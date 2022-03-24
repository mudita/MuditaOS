// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "prof.h"
#include <stdlib.h>
#include <string.h>

struct prof_pool
{
    struct task_prof_data *pool;
    unsigned int _overflow_counter;
    unsigned int _pos;

    struct prof_pool_init_data data;
    void (*clean)();
    void (*handle_overflow)(uint32_t id);
    struct task_prof_data* (*get)(uint32_t id);
};


static struct prof_pool pool = { .pool=NULL, ._overflow_counter=0, ._pos=0,.data = {0}, .clean = NULL, .handle_overflow = NULL, .get = NULL};

static void _pool_clean()
{
    memset(pool.pool, 0, sizeof(*pool.pool)*pool.data.size);
    pool._overflow_counter =0;
    pool._pos = 0;
}

static void _pool_overflow(uint32_t id)
{
    pool._overflow_counter = id;
}

/// just meant to be fast get of element
static struct task_prof_data* _pool_get(uint32_t id)
{
    if ( pool._pos == pool.data.size ) {
        pool.handle_overflow(id);
    }
    for ( size_t i =0; i < pool.data.size && i != pool._pos; ++i ) {
        if (id == pool.pool[i].task_TCB_id) {
            return &pool.pool[i];
        }
    }
    struct task_prof_data* p = &pool.pool[pool._pos];
    pool._pos++;
    return p;
}



void prof_pool_init(struct prof_pool_init_data init)
{
    pool.data = init;
    pool.clean = _pool_clean;
    pool.handle_overflow = _pool_overflow;
    pool.get = _pool_get;

    pool.pool = (struct task_prof_data *)(malloc(sizeof(struct task_prof_data)*pool.data.size));
    pool.clean();
}

void prof_pool_deinit()
{
    free(pool.pool);
    pool.pool = NULL;
}


struct prof_pool_init_data prof_pool_get_data()
{
    return pool.data;
}

void prof_pool_data_set(uint8_t ts, uint32_t id)
{
    if (pool.get == NULL ) {
        return;
    }
    struct task_prof_data *what = pool.get(id);
    if ( what == NULL) {
        return;
    }
    what->task_TCB_id=id;
    what->exec_time += ts;
    ++(what->switches);
}

unsigned int prof_pool_overflow()
{
    return pool._overflow_counter;
}

unsigned int prof_pool_flush(struct task_prof_data *mem, size_t cap)
{
    unsigned int to_ret = pool._overflow_counter;
    memcpy(mem, pool.pool, cap * (sizeof(struct task_prof_data)));
    pool.clean();
    return to_ret;
}
