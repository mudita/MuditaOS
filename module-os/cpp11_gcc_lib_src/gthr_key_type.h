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

#ifndef __FREERTOS_GTHR_KEY_KEY_H__
#define __FREERTOS_GTHR_KEY_KEY_H__

#include "thread_gthread.h"
#include <mutex>
#include <unordered_map>

namespace free_rtos_std
{

    struct Key
    {
        using __gthread_t = free_rtos_std::gthr_freertos;
        typedef void (*DestructorFoo)(void *);

        Key() = delete;
        explicit Key(DestructorFoo des) : _desFoo{des}
        {
        }

        void CallDestructor(__gthread_t::native_task_type task)
        {
            void *val;

            {
                std::lock_guard<std::mutex> lg{_mtx};

                auto item{_specValue.find(task)};
                if (item == _specValue.end())
                    return;

                val = const_cast<void *>(item->second);
                _specValue.erase(item);
            }

            if (_desFoo && val)
                _desFoo(val);
        }

        std::mutex _mtx;
        DestructorFoo _desFoo;
        std::unordered_map<__gthread_t::native_task_type, const void *> _specValue;
    };

} // namespace free_rtos_std

#endif //__FREERTOS_GTHR_KEY_KEY_H__
