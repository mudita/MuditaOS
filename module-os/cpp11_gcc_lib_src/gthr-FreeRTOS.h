/// @file
///
/// @author: Piotr Grygorczuk grygorek@gmail.com
///
/// @copyright Copyright 2019 Piotr Grygorczuk
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without
/// modification, are permitted provided that the following conditions are met:
///
/// o Redistributions of source code must retain the above copyright notice,
///   this list of conditions and the following disclaimer.
///
/// o Redistributions in binary form must reproduce the above copyright notice,
///   this list of conditions and the following disclaimer in the documentation
///   and/or other materials provided with the distribution.
///
/// o My name may not be used to endorse or promote products derived from this
///   software without specific prior written permission.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
/// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
/// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
/// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
/// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
/// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
/// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
/// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
/// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
/// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
/// POSSIBILITY OF SUCH DAMAGE.
///

#ifndef _GTHR_FREERTOS_X__H_
#define _GTHR_FREERTOS_X__H_

#include "FreeRTOS.h"
#include "condition_variable.h"
#include "gthr_key.h"
#include "semphr.h"
#include "task.h"
#include "thread_gthread.h"

#include <sys/time.h>

typedef free_rtos_std::gthr_freertos __gthread_t;

extern "C"
{

#define _GLIBCXX_HAS_GTHREADS
#define __GTHREADS 1

    // returns: 1 - thread system is active; 0 - thread system is not active
    static int __gthread_active_p()
    {
        return 1;
    }

    typedef free_rtos_std::Key *__gthread_key_t;
    typedef int __gthread_once_t;
    typedef SemaphoreHandle_t __gthread_mutex_t;
    typedef SemaphoreHandle_t __gthread_recursive_mutex_t;
    typedef free_rtos_std::cv_task_list __gthread_cond_t;

#define __GTHREAD_ONCE_INIT 0

    static inline void __GTHREAD_RECURSIVE_MUTEX_INIT_FUNCTION(__gthread_recursive_mutex_t *mutex)
    {
        *mutex = xSemaphoreCreateRecursiveMutex();
    }
    static inline void __GTHREAD_MUTEX_INIT_FUNCTION(__gthread_mutex_t *mutex)
    {
        *mutex = xSemaphoreCreateMutex();
    }

    static int __gthread_once(__gthread_once_t *once, void (*func)(void))
    {
        static __gthread_mutex_t s_m = xSemaphoreCreateMutex();
        if (!s_m)
            return 12; // POSIX error: ENOMEM

        __gthread_once_t flag{true};
        xSemaphoreTakeRecursive(s_m, portMAX_DELAY);
        std::swap(*once, flag);
        xSemaphoreGiveRecursive(s_m);

        if (flag == false)
            func();

        return 0;
    }

    static int __gthread_key_create(__gthread_key_t *keyp, void (*dtor)(void *))
    {
        return free_rtos_std::freertos_gthread_key_create(keyp, dtor);
    }

    static int __gthread_key_delete(__gthread_key_t key)
    {
        return free_rtos_std::freertos_gthread_key_delete(key);
    }

    static void *__gthread_getspecific(__gthread_key_t key)
    {
        return free_rtos_std::freertos_gthread_getspecific(key);
    }

    static int __gthread_setspecific(__gthread_key_t key, const void *ptr)
    {
        return free_rtos_std::freertos_gthread_setspecific(key, ptr);
    }
    //////////

    //////////
    static inline int __gthread_mutex_destroy(__gthread_mutex_t *mutex)
    {
        vSemaphoreDelete(*mutex);
        return 0;
    }
    static inline int __gthread_recursive_mutex_destroy(__gthread_recursive_mutex_t *mutex)
    {
        vSemaphoreDelete(*mutex);
        return 0;
    }

    static inline int __gthread_mutex_lock(__gthread_mutex_t *mutex)
    {
        return (xSemaphoreTake(*mutex, portMAX_DELAY) == pdTRUE) ? 0 : 1;
    }
    static inline int __gthread_mutex_trylock(__gthread_mutex_t *mutex)
    {
        return (xSemaphoreTake(*mutex, 0) == pdTRUE) ? 0 : 1;
    }
    static inline int __gthread_mutex_unlock(__gthread_mutex_t *mutex)
    {
        return (xSemaphoreGive(*mutex) == pdTRUE) ? 0 : 1;
    }

    static inline int __gthread_recursive_mutex_lock(__gthread_recursive_mutex_t *mutex)
    {
        return xSemaphoreTakeRecursive(*mutex, portMAX_DELAY);
    }
    static inline int __gthread_recursive_mutex_trylock(__gthread_recursive_mutex_t *mutex)
    {
        return xSemaphoreTakeRecursive(*mutex, 0);
    }
    static inline int __gthread_recursive_mutex_unlock(__gthread_recursive_mutex_t *mutex)
    {
        return xSemaphoreGiveRecursive(*mutex);
    }
////////////

////////////
#define __GTHREADS_CXX0X 1

    struct __gthread_time_t
    {
        long sec;
        long nsec;
        int64_t milliseconds() const
        {
            return int64_t(sec) * 1000 + nsec / 1000000;
        }
    };

    static inline __gthread_time_t operator-(const __gthread_time_t &lhs, const timeval &rhs)
    {
        int32_t s{lhs.sec - rhs.tv_sec};
        int32_t ns{lhs.nsec - rhs.tv_usec * 1000};

        return __gthread_time_t{s, ns};
    }

    static inline int __gthread_mutex_timedlock(__gthread_mutex_t *m, const __gthread_time_t *abs_timeout)
    {
        timeval now{};
        gettimeofday(&now, NULL);

        auto t = (*abs_timeout - now).milliseconds();
        return xSemaphoreTake(*m, pdMS_TO_TICKS(t));
    }

    static inline int __gthread_recursive_mutex_timedlock(__gthread_recursive_mutex_t *m, const __gthread_time_t *abs_time)
    {
        timeval now{};
        gettimeofday(&now, NULL);

        auto t = (*abs_time - now).milliseconds();
        return xSemaphoreTakeRecursive(*m, pdMS_TO_TICKS(t));
    }

    // All functions returning int should return zero on success or the error
    //    number.  If the operation is not supported, -1 is returned.

    static inline int __gthread_create(__gthread_t *thread, void (*func)(void *), void *args)
    {
        return thread->create_thread(func, args) ? 0 : 1;
    }
    static inline int __gthread_join(__gthread_t &thread, void **value_ptr)
    {
        thread.join();
        return 0;
    }
    static inline int __gthread_detach(__gthread_t &thread)
    {
        thread.detach();
        return 0;
    }
    static inline int __gthread_equal(const __gthread_t &t1, const __gthread_t &t2)
    {
        return t1 == t2 ? 0 : 1;
    }
    static inline __gthread_t __gthread_self(void)
    {
        return __gthread_t::self();
    }

#define _GLIBCXX_USE_SCHED_YIELD

    static inline int __gthread_yield(void)
    {
        taskYIELD();
        return 0;
    }

    //      not used - condition_variable has its own 'notify' function
    // static inline int __gthread_cond_signal(__gthread_cond_t* cond) {
    //  return -1;
    //}

    static inline int __gthread_cond_timedwait(__gthread_cond_t *cond, __gthread_mutex_t *mutex, const __gthread_time_t *abs_timeout)
    {
        auto this_thrd_hndl{__gthread_t::native_task_handle()};
        cond->lock();
        cond->push(this_thrd_hndl);
        cond->unlock();

        timeval now{};
        gettimeofday(&now, NULL);

        auto ms{(*abs_timeout - now).milliseconds()};

        __gthread_mutex_unlock(mutex);
        auto fTimeout{0 == ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(ms))};
        __gthread_mutex_lock(mutex);

        int result{0};
        if (fTimeout)
        { // timeout - remove the thread from the waiting list
            cond->lock();
            cond->remove(this_thrd_hndl);
            cond->unlock();
            result = 138; // posix ETIMEDOUT
        }

        return result;
    }

} // extern "C"

#endif // _GTHR_FREERTOS_X__H_
