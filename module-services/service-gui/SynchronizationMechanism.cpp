// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SynchronizationMechanism.hpp"
#include <stdexcept>

namespace service::gui
{
    class FreeRtosSynchronization : public SynchronizationMechanism
    {
      public:
        FreeRtosSynchronization();
        FreeRtosSynchronization(const FreeRtosSynchronization &)     = delete;
        FreeRtosSynchronization(FreeRtosSynchronization &&) noexcept = default;
        FreeRtosSynchronization &operator=(const FreeRtosSynchronization &) = delete;
        FreeRtosSynchronization &operator=(FreeRtosSynchronization &&) noexcept = default;
        ~FreeRtosSynchronization() noexcept override;

        void wait(cpp_freertos::Mutex &lock, std::chrono::milliseconds timeout, Predicate predicate) override;
        void notify() override;

      private:
        SemaphoreHandle_t semaphore;
    };

    FreeRtosSynchronization::FreeRtosSynchronization() : semaphore{xSemaphoreCreateBinary()}
    {}

    FreeRtosSynchronization::~FreeRtosSynchronization() noexcept
    {
        vSemaphoreDelete(semaphore);
    }

    void FreeRtosSynchronization::wait(cpp_freertos::Mutex &lock,
                                       std::chrono::milliseconds timeout,
                                       Predicate predicate)
    {
        if (!predicate) {
            // Predicate needed to cover spurious wake-ups.
            throw std::invalid_argument{"Null predicate passed."};
        }

        while (!predicate()) {
            lock.Unlock();
            xSemaphoreTake(semaphore, timeout.count());
            lock.Lock();
        }
    }

    void FreeRtosSynchronization::notify()
    {
        xSemaphoreGive(semaphore);
    }

    std::unique_ptr<SynchronizationMechanism> getFreeRtosSynchronizationMechanism()
    {
        return std::make_unique<FreeRtosSynchronization>();
    }
} // namespace service::gui
