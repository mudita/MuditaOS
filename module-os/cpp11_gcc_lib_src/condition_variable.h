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

#ifndef GTHR_FREERTOS_INTERNAL_CV_H
#define GTHR_FREERTOS_INTERNAL_CV_H

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "thread_gthread.h"

#include <algorithm>
#include <list>

namespace free_rtos_std
{

    class semaphore
    {
      public:
        semaphore()
        {
            vSemaphoreCreateBinary(_xSemaphore);
            if (!_xSemaphore)
                std::__throw_system_error(12); // POSIX error: no memory
        }

        void lock()
        {
            xSemaphoreTake(_xSemaphore, portMAX_DELAY);
        }
        void unlock()
        {
            xSemaphoreGive(_xSemaphore);
        }

        ~semaphore()
        {
            vSemaphoreDelete(_xSemaphore);
        }

        semaphore(const semaphore &) = delete;
        semaphore(semaphore &&) = delete;
        semaphore &operator=(semaphore &) = delete;
        semaphore &operator=(semaphore &&) = delete;

      private:
        SemaphoreHandle_t _xSemaphore;
    };

    // Internal free rtos task's container to support condition variable.
    // Condition variable must know all the threads waiting in a queue.
    //
    class cv_task_list
    {
      public:
        using __gthread_t = free_rtos_std::gthr_freertos;
        using thrd_type = __gthread_t::native_task_type;
        using queue_type = std::list<thrd_type>;

        cv_task_list() = default;

        void remove(thrd_type thrd)
        {
            _que.remove(thrd);
        }
        void push(thrd_type thrd)
        {
            _que.push_back(thrd);
        }
        void pop()
        {
            _que.pop_front();
        }
        bool empty() const
        {
            return _que.empty();
        }

        ~cv_task_list()
        {
            lock();
            _que = queue_type{};
            unlock();
        }

        // no copy and no move
        cv_task_list &operator=(const cv_task_list &r) = delete;
        cv_task_list &operator=(cv_task_list &&r) = delete;
        cv_task_list(cv_task_list &&) = delete;
        cv_task_list(const cv_task_list &) = delete;

        thrd_type &front()
        {
            return _que.front();
        }
        const thrd_type &front() const
        {
            return _que.front();
        }
        thrd_type &back()
        {
            return _que.back();
        }
        const thrd_type &back() const
        {
            return _que.back();
        }

        void lock()
        {
            _sem.lock();
        }
        void unlock()
        {
            _sem.unlock();
        }

      private:
        queue_type _que;
        semaphore _sem;
    };
} // namespace free_rtos_std

#endif // GTHR_FREERTOS_INTERNAL_CV_H
