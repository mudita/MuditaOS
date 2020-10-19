/*
 * =====================================================================================
 *
 *       Filename:  freertos-tls.cpp
 *
 *    Description: Free rtos thread local storage wrapper
 *
 *        Version:  1.0
 *        Created:  16.10.2020 09:48:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *
 * =====================================================================================
 */

#include <thread.hpp>

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
