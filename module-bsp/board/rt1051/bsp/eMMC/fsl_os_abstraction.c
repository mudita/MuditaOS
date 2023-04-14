#include "fsl_os_abstraction.h"
#include "fsl_common.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"

osa_status_t OSA_SemaphoreCreate(osa_semaphore_handle_t semaphoreHandle, uint32_t initValue)
{
    semaphoreHandle = xSemaphoreCreateBinary();
    if (semaphoreHandle == NULL) {
        return KOSA_StatusError;
    }
    xSemaphoreGive(semaphoreHandle);
    return KOSA_StatusSuccess;
}

osa_status_t OSA_SemaphoreWait(osa_semaphore_handle_t semaphoreHandle, uint32_t millisec)
{
    if (!semaphoreHandle) {
        return KOSA_StatusError;
    }
    if (xSemaphoreTake(semaphoreHandle, pdMS_TO_TICKS(millisec)) == pdTRUE) {
        return KOSA_StatusSuccess;
    }
    else {
        return KOSA_StatusTimeout;
    }
}

osa_status_t OSA_SemaphorePost(osa_semaphore_handle_t semaphoreHandle)
{
    if (!semaphoreHandle) {
        return KOSA_StatusError;
    }
    if (xSemaphoreGive(semaphoreHandle) == pdTRUE) {
        return KOSA_StatusSuccess;
    }
    else {
        return KOSA_StatusError;
    }
}

osa_status_t OSA_SemaphoreDestroy(osa_semaphore_handle_t semaphoreHandle)
{
    if (!semaphoreHandle) {
        return KOSA_StatusError;
    }
    vSemaphoreDelete(semaphoreHandle);
    semaphoreHandle = NULL;
    return KOSA_StatusSuccess;
}

osa_status_t OSA_MutexCreate(osa_mutex_handle_t mutexHandle)
{
    mutexHandle = xSemaphoreCreateMutex();
    if (mutexHandle == NULL) {
        return KOSA_StatusError;
    }
    return KOSA_StatusSuccess;
}

osa_status_t OSA_MutexLock(osa_mutex_handle_t mutexHandle, uint32_t millisec)
{
    if (!mutexHandle) {
        return KOSA_StatusError;
    }
    if (xSemaphoreTake(mutexHandle, pdMS_TO_TICKS(millisec)) == pdTRUE) {
        return KOSA_StatusSuccess;
    }
    else {
        return KOSA_StatusTimeout;
    }
}

osa_status_t OSA_MutexUnlock(osa_mutex_handle_t mutexHandle)
{
    if (!mutexHandle) {
        return KOSA_StatusError;
    }
    if (xSemaphoreGive(mutexHandle) == pdTRUE) {
        return KOSA_StatusSuccess;
    }
    else {
        return KOSA_StatusError;
    }
}


osa_status_t OSA_MutexDestroy(osa_mutex_handle_t mutexHandle)
{
    if (!mutexHandle) {
        return KOSA_StatusError;
    }
    vSemaphoreDelete(mutexHandle);
    mutexHandle = NULL;
    return KOSA_StatusSuccess;
}

void OSA_EnterCritical(uint32_t *sr)
{
    taskENTER_CRITICAL();
}

void OSA_ExitCritical(uint32_t sr)
{
    taskEXIT_CRITICAL();
}

void OSA_TimeDelay(uint32_t millisec)
{
    vTaskDelay(pdMS_TO_TICKS(millisec));
}