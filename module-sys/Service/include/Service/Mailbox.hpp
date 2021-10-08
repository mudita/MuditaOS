// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <deque>
#include "thread.hpp"
#include <mutex.hpp>
#include <condition_variable.hpp>

template <typename T> class Mailbox
{
  public:
    Mailbox(cpp_freertos::Thread *thread) : thread_(thread)
    {}

    T peek()
    {

        cpp_freertos::LockGuard mlock(mutex_);
        while (queue_.empty()) {
            thread_->Wait(cond_, mutex_);
        }
        auto item = queue_.front();
        return item;
    }

    T pop(uint32_t timeout = portMAX_DELAY)
    {

        cpp_freertos::LockGuard mlock(mutex_);
        while (queue_.empty()) {
            if (thread_->Wait(cond_, mutex_, timeout) == false) {
                return nullptr;
            }
        }
        auto item = queue_.front();
        queue_.pop_front();
        return item;
    }

    void pop(T &item)
    {
        cpp_freertos::LockGuard mlock(mutex_);
        while (queue_.empty()) {
            thread_->Wait(cond_, mutex_);
        }
        item = queue_.front();
        queue_.pop_front();
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
        cond_.Signal();
    }

    void push(T &&item)
    {
        mutex_.Lock();
        queue_.push_back(std::move(item));
        mutex_.Unlock();
        cond_.Signal();
    }

  private:
    cpp_freertos::Thread *thread_;
    std::deque<T> queue_;
    cpp_freertos::MutexStandard mutex_;
    cpp_freertos::ConditionVariable cond_;
};
