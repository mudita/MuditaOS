// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <RTOSWrapper/include/thread.hpp>
#include "../FrameStack.hpp"

using namespace service::renderer;

class DummyThread : public cpp_freertos::Thread
{
  public:
    std::function<void()> foo;

    DummyThread(std::function<void()> foo) : cpp_freertos::Thread("Dummy", pow(2, 8), 1), foo(foo)
    {}

    void Run() override
    {
        if (foo) {
            foo();
        }
        cpp_freertos::Thread::EndScheduler();
    }
};

TEST_CASE("Trylock")
{
    std::function<void()> locktest = []() {
        cpp_freertos::MutexStandard mutex;
        auto lock1 = Trylock(mutex, 10000);
        REQUIRE(lock1.isLocked() == true);
        auto lock2 = Trylock(mutex, 1);
        REQUIRE(lock2.isLocked() == false);
    };

    auto t = DummyThread(locktest);
    t.Start();
    cpp_freertos::Thread::StartScheduler();
}
