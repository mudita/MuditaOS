// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <SynchronizationMechanism.hpp>

#include "mutex.hpp"

#include <chrono>
#include <thread>

class MockedSynchronizationMechanism : public service::gui::SynchronizationMechanism
{
  public:
    void wait(cpp_freertos::Mutex &lock, std::chrono::milliseconds timeout, Predicate predicate) override;
    void notify() override;
};

void MockedSynchronizationMechanism::wait(cpp_freertos::Mutex &lock,
                                          std::chrono::milliseconds timeout,
                                          Predicate predicate)
{
    using namespace std::chrono_literals;
    while (!predicate()) {
        lock.Unlock();
        std::this_thread::sleep_for(10ms);
        lock.Lock();
    }
}

void MockedSynchronizationMechanism::notify()
{}
