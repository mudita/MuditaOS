#include "fsl_common.h"

#define osaWaitForever_c         ((uint32_t)(-1))
#define OSA_SEM_HANDLE_SIZE   (4U)
#define OSA_MUTEX_HANDLE_SIZE (4U)
#define OSA_SEMAPHORE_HANDLE_DEFINE(name) \
    uint32_t name[(OSA_SEM_HANDLE_SIZE + sizeof(uint32_t) - 1U) / sizeof(uint32_t)]
#define OSA_MUTEX_HANDLE_DEFINE(name) uint32_t name[(OSA_MUTEX_HANDLE_SIZE + sizeof(uint32_t) - 1U) / sizeof(uint32_t)]
#define OSA_SR_ALLOC() uint32_t osaCurrentSr = 0U;
/*
 * Enter critical mode
 */
#define OSA_ENTER_CRITICAL() OSA_EnterCritical(&osaCurrentSr)
/*
 * Exit critical mode and retore the previous mode
 */
#define OSA_EXIT_CRITICAL() OSA_ExitCritical(osaCurrentSr)

/*! @brief Defines the return status of OSA's functions */
#if (defined(SDK_COMPONENT_DEPENDENCY_FSL_COMMON) && (SDK_COMPONENT_DEPENDENCY_FSL_COMMON > 0U))
typedef enum _osa_status
{
    KOSA_StatusSuccess = kStatus_Success,                  /*!< Success */
    KOSA_StatusError   = MAKE_STATUS(kStatusGroup_OSA, 1), /*!< Failed */
    KOSA_StatusTimeout = MAKE_STATUS(kStatusGroup_OSA, 2), /*!< Timeout occurs while waiting */
    KOSA_StatusIdle    = MAKE_STATUS(kStatusGroup_OSA, 3), /*!< Used for bare metal only, the wait object is not ready
                                                                 and timeout still not occur */
} osa_status_t;
#else
typedef enum _osa_status
{
    KOSA_StatusSuccess = 0, /*!< Success */
    KOSA_StatusError   = 1, /*!< Failed */
    KOSA_StatusTimeout = 2, /*!< Timeout occurs while waiting */
    KOSA_StatusIdle    = 3, /*!< Used for bare metal only, the wait object is not ready
                                                and timeout still not occur */
} osa_status_t;

#endif

typedef void *osa_semaphore_handle_t;
typedef void *osa_mutex_handle_t;

/*!
 * @brief Enter critical with nesting mode.
 *
 * @param sr Store current status and return to caller.
 */
void OSA_EnterCritical(uint32_t *sr);

/*!
 * @brief Exit critical with nesting mode.
 *
 * @param sr Previous status to restore.
 */
void OSA_ExitCritical(uint32_t sr);

/*!
 * @brief Creates a semaphore with a given value.
 *
 * This function creates a semaphore and sets the value to the parameter
 * initValue.
 *
 * Example below shows how to use this API to create the semaphore handle.
 * @code
 *   OSA_SEMAPHORE_HANDLE_DEFINE(semaphoreHandle);
 *   OSA_SemaphoreCreate((osa_semaphore_handle_t)semaphoreHandle, 0xff);
 * @endcode
 *
 * @param semaphoreHandle Pointer to a memory space of size OSA_SEM_HANDLE_SIZE allocated by the caller.
 * The handle should be 4 byte aligned, because unaligned access doesn't be supported on some devices.
 * You can define the handle in the following two ways:
 * #OSA_SEMAPHORE_HANDLE_DEFINE(semaphoreHandle);
 * or
 * uint32_t semaphoreHandle[((OSA_SEM_HANDLE_SIZE + sizeof(uint32_t) - 1U) / sizeof(uint32_t))];
 * @param initValue Initial value the semaphore will be set to.
 *
 * @retval KOSA_StatusSuccess  the new semaphore if the semaphore is created successfully.
 * @retval KOSA_StatusError   if the semaphore can not be created.
 */
osa_status_t OSA_SemaphoreCreate(osa_semaphore_handle_t semaphoreHandle, uint32_t initValue);

/*!
 * @brief Pending a semaphore with timeout.
 *
 * This function checks the semaphore's counting value. If it is positive,
 * decreases it and returns KOSA_StatusSuccess. Otherwise, a timeout is used
 * to wait.
 *
 * @param semaphoreHandle    The semaphore handle.
 * @param millisec The maximum number of milliseconds to wait if semaphore is not
 *                 positive. Pass osaWaitForever_c to wait indefinitely, pass 0
 *                 will return KOSA_StatusTimeout immediately.
 *
 * @retval KOSA_StatusSuccess  The semaphore is received.
 * @retval KOSA_StatusTimeout  The semaphore is not received within the specified 'timeout'.
 * @retval KOSA_StatusError    An incorrect parameter was passed.
 */
osa_status_t OSA_SemaphoreWait(osa_semaphore_handle_t semaphoreHandle, uint32_t millisec);

/*!
 * @brief Signals for someone waiting on the semaphore to wake up.
 *
 * Wakes up one task that is waiting on the semaphore. If no task is waiting, increases
 * the semaphore's counting value.
 *
 * @param semaphoreHandle The semaphore handle to signal.
 *
 * @retval KOSA_StatusSuccess The semaphore is successfully signaled.
 * @retval KOSA_StatusError   The object can not be signaled or invalid parameter.
 *
 */
osa_status_t OSA_SemaphorePost(osa_semaphore_handle_t semaphoreHandle);

/*!
 * @brief Destroys a previously created semaphore.
 *
 * @param semaphoreHandle The semaphore handle.
 * The macro SEMAPHORE_HANDLE_BUFFER_GET is used to get the semaphore buffer pointer,
 * and should not be used before the macro SEMAPHORE_HANDLE_BUFFER_DEFINE is used.
 *
 * @retval KOSA_StatusSuccess The semaphore is successfully destroyed.
 * @retval KOSA_StatusError   The semaphore can not be destroyed.
 */
osa_status_t OSA_SemaphoreDestroy(osa_semaphore_handle_t semaphoreHandle);

/*!
 * @brief Create an unlocked mutex.
 *
 * This function creates a non-recursive mutex and sets it to unlocked status.
 *
 * Example below shows how to use this API to create the mutex handle.
 * @code
 *   OSA_MUTEX_HANDLE_DEFINE(mutexHandle);
 *   OSA_MutexCreate((osa_mutex_handle_t)mutexHandle);
 * @endcode
 *
 * @param mutexHandle       Pointer to a memory space of size OSA_MUTEX_HANDLE_SIZE allocated by the caller.
 * The handle should be 4 byte aligned, because unaligned access doesn't be supported on some devices.
 * You can define the handle in the following two ways:
 * #OSA_MUTEX_HANDLE_DEFINE(mutexHandle);
 * or
 * uint32_t mutexHandle[((OSA_MUTEX_HANDLE_SIZE + sizeof(uint32_t) - 1U) / sizeof(uint32_t))];
 * @retval KOSA_StatusSuccess  the new mutex if the mutex is created successfully.
 * @retval KOSA_StatusError   if the mutex can not be created.
 */
osa_status_t OSA_MutexCreate(osa_mutex_handle_t mutexHandle);

/*!
 * @brief Waits for a mutex and locks it.
 *
 * This function checks the mutex's status. If it is unlocked, locks it and returns the
 * KOSA_StatusSuccess. Otherwise, waits for a timeout in milliseconds to lock.
 *
 * @param mutexHandle The mutex handle.
 * @param millisec The maximum number of milliseconds to wait for the mutex.
 *                 If the mutex is locked, Pass the value osaWaitForever_c will
 *                 wait indefinitely, pass 0 will return KOSA_StatusTimeout
 *                 immediately.
 *
 * @retval KOSA_StatusSuccess The mutex is locked successfully.
 * @retval KOSA_StatusTimeout Timeout occurred.
 * @retval KOSA_StatusError   Incorrect parameter was passed.
 *
 * @note This is non-recursive mutex, a task can not try to lock the mutex it has locked.
 */
osa_status_t OSA_MutexLock(osa_mutex_handle_t mutexHandle, uint32_t millisec);

/*!
 * @brief Unlocks a previously locked mutex.
 *
 * @param mutexHandle The mutex handle.
 *
 * @retval KOSA_StatusSuccess The mutex is successfully unlocked.
 * @retval KOSA_StatusError   The mutex can not be unlocked or invalid parameter.
 */
osa_status_t OSA_MutexUnlock(osa_mutex_handle_t mutexHandle);

/*!
 * @brief Destroys a previously created mutex.
 *
 * @param mutexHandle The mutex handle.
 *
 * @retval KOSA_StatusSuccess The mutex is successfully destroyed.
 * @retval KOSA_StatusError   The mutex can not be destroyed.
 *
 */
osa_status_t OSA_MutexDestroy(osa_mutex_handle_t mutexHandle);

/*!
 * @brief Delays execution for a number of milliseconds.
 *
 * @param millisec The time in milliseconds to wait.
 */
void OSA_TimeDelay(uint32_t millisec);
