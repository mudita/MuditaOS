/*
 * Mailbox.hpp
 *
 *  Created on: Mar 7, 2019
 *      Author: mati
 */

#ifndef SYSTEMMANAGER_MAILBOX_HPP_
#define SYSTEMMANAGER_MAILBOX_HPP_

#include <deque>
#include "thread.hpp"
#include <mutex.hpp>
#include <condition_variable.hpp>

using namespace cpp_freertos;

template <typename T> class Mailbox
{
  public:
    Mailbox(Thread *thread) : thread_(thread)
    {}

    T peek()
    {

        LockGuard mlock(mutex_);
        while (queue_.empty()) {
            thread_->Wait(cond_, mutex_);
        }
        auto item = queue_.front();
        return item;
    }

    T pop(uint32_t timeout = portMAX_DELAY)
    {

        LockGuard mlock(mutex_);
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
        LockGuard mlock(mutex_);
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
    Thread *thread_;
    std::deque<T> queue_;
    MutexStandard mutex_;
    ConditionVariable cond_;
};

#endif /* SYSTEMMANAGER_MAILBOX_HPP_ */
