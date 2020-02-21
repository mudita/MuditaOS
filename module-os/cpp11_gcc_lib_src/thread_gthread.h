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

#ifndef GTHR_FREERTOS_INTERNAL_H
#define GTHR_FREERTOS_INTERNAL_H

#include "FreeRTOS.h"
#include "critical_section.h"
#include "event_groups.h"
#include "task.h"

#include <exception> // std::terminate
#include <utility>   // std::forward

namespace std
{
    class thread;
}

namespace free_rtos_std
{

    class gthr_freertos
    {
        // 1. std::thread class has a single member variable representing
        //    a native thread handle
        // 2. Detach requires that the native thread function will execute
        //    even if the std::thread instance has been destroyed. The native
        //    thread function must take the ownership of any resources allocated
        //    during the thread creation. This could be the thread handle itself.
        //    Detach must make sure that the thread has started execution and
        //    the ownership has been taken.
        // 3. Join requires a way to switch the current context to a waiting
        //    state. The native thread function must have a way to unlock
        //    a joined thread.
        // 4. FreeRTOS does not have an interface implementing join. It is possible
        //    to suspend a thread but the thread we are waiting for to join is not
        //    aware which thread is waiting.  Is this statement wrong?
        // 5. Solution is to use events group.
        //    The handle life time must be handled by this class.
        // 6. Life time of thread handle and events group handle is not the same.
        //    There are two cases:
        //     a) detach is called and std::thread instance is destroyed; in this case
        //        thread function outlives the thread instance
        //     b) thread function exits first; in this case the thread instance
        //        outlives the thread function; join must have access to the events
        //        group to check whether the thread function has finished or not.
        //    So, the thread handle must exist as long as thread function executes.
        //    The events group must live as long as std::thread instance exists.
        // 7. Although the living time is different for two handles,
        //    see pt. 1 -only one member variable to handle free rtos interface.
        //    For this reason a single class is a container for the two handles.

        friend std::thread;

        enum
        {
            eEvStoragePos = 0,
            eStartedEv = 1 << 22,
            eJoinEv = 1 << 23
        };

      public:
        typedef void (*task_foo)(void *);
        typedef TaskHandle_t native_task_type;

        gthr_freertos(const gthr_freertos &r)
        {
            critical_section critical;

            _taskHandle = r._taskHandle;
            _evHandle = r._evHandle;
            _arg = r._arg;
            _fOwner = false; // it is just a copy
        }

        gthr_freertos(gthr_freertos &&r)
        {
            taskENTER_CRITICAL();
            if (r._fOwner)
            {
                taskEXIT_CRITICAL();
                // make sure it has already started
                // - native thread function must make local copies first
                r.wait_for_start();
                taskENTER_CRITICAL();
            }
            // 'this' becomes the owner if r is the owner
            move(std::forward<gthr_freertos>(r));
            taskEXIT_CRITICAL();
        }

        bool create_thread(task_foo foo, void *arg)
        {
            _arg = arg;

            _evHandle = xEventGroupCreate();
            if (!_evHandle)
                std::terminate();

            {
                critical_section critical;

                xTaskCreate(foo, "Task", 512, this, tskIDLE_PRIORITY + 1, &_taskHandle);
                if (!_taskHandle)
                    std::terminate();

                vTaskSetThreadLocalStoragePointer(_taskHandle, eEvStoragePos, _evHandle);
                _fOwner = true;
            }

            return true;
        }

        void join()
        { // note 1: _evHandle must be valid here. Even if the native thread function
            //   has finished and got destroyed the _taskHandle, it
            //   must not release the _evHandle!
            //
            // note 2: The native thread function must call notify_joined when it has
            //   finished. Even if 'join' is called after that, the bits are set
            //   and event group will not block. So it is safe to call wait here.
            //
            // note 3: Instead of keeping _evHandle, it could be possible to read
            //   it from the local storage of the task. However, that would require
            //   checking if the task still exists and use critical section if it does.
            //   It is faster to use _evHandle directly, even if it is an extra item to
            //   copy each time when this instance is copied.
            while (0 == xEventGroupWaitBits(_evHandle, eJoinEv | eStartedEv, pdFALSE, pdTRUE, portMAX_DELAY))
                ;
        }

        void detach()
        { // Detaching is removing the event's object. It can be done
            // only if the thread has started execution.
            wait_for_start();

            { // unfortunately critical section is needed here to make sure
                // the task is not deleted while accessing the task's local storage.
                critical_section critical;

                if (eDeleted != eTaskGetState(_taskHandle))
                {
                    vTaskSetThreadLocalStoragePointer(_taskHandle, eEvStoragePos, nullptr);
                    vEventGroupDelete(_evHandle);

                    // Thread still exists but detach removes ownership.
                    // Ownership belongs to the native thread. It will release the task
                    // handle once user's thread function exits.
                    _fOwner = false;
                }
            }
        }

        void notify_started()
        { // Function should be called only from the controlled task
            // and only when the (internal, not application's one) thread function
            // has started execution.
            xEventGroupSetBits(_evHandle, eStartedEv);
        }

        void notify_joined()
        { // Function should be called only from the controlled task
            // and only when the thread function has finished execution.
            {
                critical_section critical;

                // Note: The _evHandle may be invalid if thread has been detached.
                //    It is required to get the handle from the thread itself.
                //    The detach function will set this handle to nullptr.
                auto evHnd = static_cast<EventGroupHandle_t>(pvTaskGetThreadLocalStoragePointer(_taskHandle, eEvStoragePos));

                if (evHnd)
                {
                    xEventGroupSetBits(evHnd, eJoinEv);
                }
                else
                {
                    // thread has been detached, nothing to do
                }
            }

            // vTaskDelete will not return
            vTaskDelete(nullptr);
        }

        static gthr_freertos self()
        {
            auto tHnd = xTaskGetCurrentTaskHandle();
            auto evHnd = static_cast<EventGroupHandle_t>(pvTaskGetThreadLocalStoragePointer(tHnd, eEvStoragePos));
            return gthr_freertos{tHnd, evHnd};
        }

        static native_task_type native_task_handle()
        {
            return xTaskGetCurrentTaskHandle();
        }

        bool operator==(const gthr_freertos &r) const
        {
            return _taskHandle == r._taskHandle;
        }

        bool operator!=(const gthr_freertos &r) const
        {
            return !operator==(r);
        }

        bool operator<(const gthr_freertos &r) const
        {
            return _taskHandle < r._taskHandle;
        }

        void *arg() const
        {
            return _arg;
        }

        ~gthr_freertos() = default;
        gthr_freertos &operator=(const gthr_freertos &r) = delete;

      private:
        gthr_freertos() = default;

        gthr_freertos(native_task_type thnd, EventGroupHandle_t ehnd) : _taskHandle{thnd}, _evHandle{ehnd}
        {
        }

        gthr_freertos &operator=(gthr_freertos &&r)
        {
            if (this == &r)
                return *this;

            taskENTER_CRITICAL();

            if (_fOwner)
            { // If 'r' is the owner then 'this' will get the
                // new ownership. If 'r' is not the owner then
                // just a copy is being moved. Either way 'this'
                // ownership is lost and handles must be deleted.
                if (eDeleted != eTaskGetState(_taskHandle))
                    vTaskDelete(_taskHandle);
                if (_evHandle)
                    vEventGroupDelete(_evHandle);
                _fOwner = false;
            }
            else if (r._fOwner)
            {
                taskEXIT_CRITICAL();
                // make sure it has already started
                // - native thread function must make local copies first
                r.wait_for_start();
                taskENTER_CRITICAL();
            }
            // 'this' becomes the owner if r is the owner
            move(std::forward<gthr_freertos>(r));
            taskEXIT_CRITICAL();
            return *this;
        }

        void move(gthr_freertos &&r)
        {
            _taskHandle = r._taskHandle;
            _evHandle = r._evHandle;
            _arg = r._arg;
            _fOwner = r._fOwner;
            r._taskHandle = nullptr;
            r._evHandle = nullptr;
            r._arg = nullptr;
            r._fOwner = false;
        }

        void wait_for_start()
        {
            while (0 == xEventGroupWaitBits(_evHandle, eStartedEv, pdFALSE, pdTRUE, portMAX_DELAY))
                ;
        }

        native_task_type _taskHandle{nullptr};
        EventGroupHandle_t _evHandle{nullptr};
        void *_arg{nullptr};
        bool _fOwner{false};
    };

} // namespace free_rtos_std

#endif // GTHR_FREERTOS_INTERNAL_H
