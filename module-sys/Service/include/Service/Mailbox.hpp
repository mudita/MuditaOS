// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <deque>
#include "thread.hpp"
#include <mutex.hpp>
#include <condition_variable.hpp>
#include <optional>

class ServiceLock
{
    cpp_freertos::Thread *thread;
    cpp_freertos::MutexStandard &mutex_;
    cpp_freertos::ConditionVariable cond_;

  public:
    ServiceLock(cpp_freertos::Thread *thread, cpp_freertos::MutexStandard &mutex_) : thread(thread), mutex_(mutex_)
    {}

    bool wait()
    {
        return thread->Wait(cond_, mutex_);
    }

    bool wait(TickType_t ticks)
    {
        return thread->Wait(cond_, mutex_, ticks);
    }
    bool signal()
    {
        cond_.Signal();
        return true;
    }
};

template <typename T, typename Base = cpp_freertos::Thread *, typename Lock = ServiceLock> class Mailbox
{
  public:
    Mailbox(Base thread) : thread_(thread)
    {}

    std::optional<T> peek()
    {
        cpp_freertos::LockGuard mlock(mutex_);
        if (queue_.empty()) {
            return {};
        }
        auto el = queue_.front();
        queue_.pop_front();
        return {el};
    }

    T pop(uint32_t timeout = portMAX_DELAY)
    {

        cpp_freertos::LockGuard mlock(mutex_);
        while (queue_.empty()) {
            if (lock.wait(timeout) == false) {
                return nullptr;
            }
        }
        auto item = queue_.front();
        queue_.pop_front();
        return item;
    }

    void push_front(const T &item)
    {
        mutex_.Lock();
        queue_.push_front(item);
        mutex_.Unlock();
    }

    void push(const T &item)
    {
        mutex_.Lock();
        queue_.push_back(item);
        mutex_.Unlock();
        lock.signal();
    }

    void push(T &&item)
    {
        mutex_.Lock();
        queue_.push_back(std::move(item));
        mutex_.Unlock();
        lock.signal();
    }

    bool empty()
    {
        return queue_.empty();
    }

  private:
    Base thread_;
    std::deque<T> queue_;
    cpp_freertos::MutexStandard mutex_;

    Lock lock{thread_, mutex_};
};
