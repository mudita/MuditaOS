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
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include <condition_variable>

namespace std
{

    condition_variable::condition_variable() = default;

    condition_variable::~condition_variable()
    { // It is only safe to invoke the destructor if all threads have been notified.
        if (!_M_cond.empty())
            std::__throw_system_error(117); // POSIX error: structure needs cleaning
    }

    void condition_variable::wait(std::unique_lock<std::mutex> &m)
    { // pre-condition: m is taken!!
        _M_cond.lock();
        _M_cond.push(__gthread_t::native_task_handle());
        _M_cond.unlock();

        m.unlock();

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        m.lock();
    }

    void condition_variable::notify_one()
    {
        _M_cond.lock();
        if (!_M_cond.empty())
        {
            auto t = _M_cond.front();
            _M_cond.pop();
            xTaskNotifyGive(t);
        }
        _M_cond.unlock();
    }

    void condition_variable::notify_all()
    {
        _M_cond.lock();
        while (!_M_cond.empty())
        {
            auto t = _M_cond.front();
            _M_cond.pop();
            xTaskNotifyGive(t);
        }
        _M_cond.unlock();
    }

    // condition_variable -*- C++ -*-
    //
    // Copyright (C) 2008-2019 Free Software Foundation, Inc.
    //
    // This file is part of the GNU ISO C++ Library.  This library is free
    // software; you can redistribute it and/or modify it under the
    // terms of the GNU General Public License as published by the
    // Free Software Foundation; either version 3, or (at your option)
    // any later version.
    //
    // This library is distributed in the hope that it will be useful,
    // but WITHOUT ANY WARRANTY; without even the implied warranty of
    // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    // GNU General Public License for more details.
    //
    // Under Section 7 of GPL version 3, you are granted additional
    // permissions described in the GCC Runtime Library Exception, version
    // 3.1, as published by the Free Software Foundation.
    //
    // You should have received a copy of the GNU General Public License and
    // a copy of the GCC Runtime Library Exception along with this program;
    // see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
    // <http://www.gnu.org/licenses/>.
    //
    //
    //
    // Code below has been copied from gcc distribution
    // at: gcc/libstdc++-v3/src/c++11/condition_variable.cc
    //
    // It provides an implementation of std::notify_all_at_thread_exit
    // and a mechanism required by 'futures' from the C++ standard.
    //

    extern void __at_thread_exit(__at_thread_exit_elt *);

    namespace
    {
        __gthread_key_t key;

        void run(void *p)
        {
            auto elt = (__at_thread_exit_elt *)p;
            while (elt)
            {
                auto next = elt->_M_next;
                elt->_M_cb(elt);
                elt = next;
            }
        }

        void run()
        {
            auto elt = (__at_thread_exit_elt *)__gthread_getspecific(key);
            __gthread_setspecific(key, nullptr);
            run(elt);
        }

        struct notifier final : __at_thread_exit_elt
        {
            notifier(condition_variable &cv, unique_lock<mutex> &l) : cv(&cv), mx(l.release())
            {
                _M_cb = &notifier::run;
                __at_thread_exit(this);
            }

            ~notifier()
            {
                mx->unlock();
                cv->notify_all();
            }

            condition_variable *cv;
            mutex *mx;

            static void run(void *p)
            {
                delete static_cast<notifier *>(p);
            }
        };

        void key_init()
        {
            struct key_s
            {
                key_s()
                {
                    __gthread_key_create(&key, run);
                }
                ~key_s()
                {
                    __gthread_key_delete(key);
                }
            };
            static key_s ks;
            // Also make sure the callbacks are run by std::exit.
            std::atexit(run);
        }
    } // namespace

    void __at_thread_exit(__at_thread_exit_elt *elt)
    {
        static __gthread_once_t once = __GTHREAD_ONCE_INIT;
        __gthread_once(&once, key_init);

        elt->_M_next = (__at_thread_exit_elt *)__gthread_getspecific(key);
        __gthread_setspecific(key, elt);
    }

    void notify_all_at_thread_exit(condition_variable &cv, unique_lock<mutex> l)
    {
        (void)new notifier{cv, l};
    }

} // namespace std
