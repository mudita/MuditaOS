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

#include "gthr_key_type.h"
#include <cassert>

namespace free_rtos_std
{

    Key *s_key;

    int freertos_gthread_key_create(Key **keyp, void (*dtor)(void *))
    {
        // There is only one key for all threads. If more keys are needed
        // a list must be implemented.
        assert(!s_key);
        s_key = new Key(dtor);

        *keyp = s_key;
        return 0;
    }

    int freertos_gthread_key_delete(Key *)
    {
        // no synchronization here:
        //   It is up to the applicaiton to delete (or maintain a reference)
        //   the thread specific data associated with the key.
        delete s_key;
        s_key = nullptr;
        return 0;
    }

    void *freertos_gthread_getspecific(Key *key)
    {
        std::lock_guard<std::mutex> lg{key->_mtx};

        auto item = key->_specValue.find(__gthread_t::self().native_task_handle());
        if (item == key->_specValue.end())
            return nullptr;
        return const_cast<void *>(item->second);
    }

    int freertos_gthread_setspecific(Key *key, const void *ptr)
    {
        std::lock_guard<std::mutex> lg{key->_mtx};

        auto &cont{key->_specValue};
        auto task{__gthread_t::self().native_task_handle()};
        if (ptr)
            cont[task] = ptr;
        else
            (void)cont.erase(task);
        return 0;
    }

} // namespace free_rtos_std
