// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

typedef long BaseType_t;
typedef void *TaskHandle_t;
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 5

namespace
{
    thread_local void *tls_pointers[configNUM_THREAD_LOCAL_STORAGE_POINTERS];
}

extern "C"
{

    void *ff_stdio_pvTaskGetThreadLocalStoragePointer(TaskHandle_t, BaseType_t xIndex)
    {
        return tls_pointers[xIndex];
    }
    void ff_stdio_vTaskSetThreadLocalStoragePointer(TaskHandle_t, BaseType_t xIndex, void *pvValue)
    {
        tls_pointers[xIndex] = pvValue;
    }
}
