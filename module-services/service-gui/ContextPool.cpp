// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ContextPool.hpp"

#include <mutex.hpp>

#include <algorithm>

namespace service::gui
{
    using namespace std::literals::chrono_literals;

    namespace
    {
        std::unique_ptr<::gui::Context> allocateContext(::gui::Size screenSize)
        {
            return std::make_unique<::gui::Context>(screenSize.width, screenSize.height);
        }

        constexpr auto DefaultWaitTime = 5000ms;
    } // namespace

    class FreeRtosSynchronization : public SynchronizationMechanism
    {
      public:
        FreeRtosSynchronization();

        void wait(cpp_freertos::Mutex &lock, std::chrono::milliseconds timeout, Predicate predicate) override;
        void notify() override;

      private:
        SemaphoreHandle_t semaphore;
    };

    FreeRtosSynchronization::FreeRtosSynchronization() : semaphore{xSemaphoreCreateBinary()}
    {}

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

    std::unique_ptr<SynchronizationMechanism> getDefaultContextPoolSynchronizationMechanism()
    {
        return std::make_unique<FreeRtosSynchronization>();
    }

    ContextPool::ContextPool(::gui::Size screenSize,
                             std::size_t capacity,
                             std::unique_ptr<SynchronizationMechanism> &&synchronization)
        : synchronization{std::move(synchronization)}
    {
        contexts.reserve(capacity);
        freeContextIds.reserve(capacity);
        lockedContextIds.reserve(capacity);

        for (std::size_t i = 0; i < capacity; ++i) {
            contexts.push_back(allocateContext(screenSize));
            freeContextIds.push_back(i);
        }
    }

    ContextPool::~ContextPool() noexcept = default;

    auto ContextPool::peakContext(int id) -> const ::gui::Context *
    {
        return contexts[id].get();
    }

    auto ContextPool::borrowContext() noexcept -> std::pair<int, ::gui::Context *>
    {
        cpp_freertos::LockGuard lock(mutex);
        synchronization->wait(mutex, DefaultWaitTime, [this]() { return isAvailable(); });

        const auto contextId = freeContextIds.front();
        lockContext(contextId);
        return std::make_pair(contextId, contexts[contextId].get());
    }

    auto ContextPool::isAvailable() const noexcept -> bool
    {
        return !freeContextIds.empty();
    }

    void ContextPool::lockContext(int contextId)
    {
        removeContextId(freeContextIds, contextId);
        addContextId(lockedContextIds, contextId);
    }

    void ContextPool::returnContext(int id) noexcept
    {
        cpp_freertos::LockGuard lock(mutex);
        freeContext(id);

        synchronization->notify();
    }

    void ContextPool::freeContext(int contextId)
    {
        removeContextId(lockedContextIds, contextId);
        addContextId(freeContextIds, contextId);
    }

    void ContextPool::removeContextId(std::vector<int> &sequence, int contextId)
    {
        sequence.erase(std::remove(sequence.begin(), sequence.end(), contextId), sequence.end());
    }

    void ContextPool::addContextId(std::vector<int> &sequence, int contextId)
    {
        if (const auto it = std::find(sequence.begin(), sequence.end(), contextId); it == sequence.end()) {
            // Add if not found.
            sequence.push_back(contextId);
        }
    }
} // namespace service::gui
