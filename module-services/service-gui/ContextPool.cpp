// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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

    auto ContextPool::peekContext(int id) noexcept -> ::gui::Context *
    {
        return contexts[id].get();
    }

    auto ContextPool::borrowContext() -> std::pair<int, ::gui::Context *>
    {
        cpp_freertos::LockGuard lock(mutex);
        synchronization->wait(mutex, DefaultWaitTime, [this]() { return isAvailable(); });

        const auto contextId = freeContextIds.front();
        lockContext(contextId);
        return std::make_pair(contextId, contexts[contextId].get());
    }

    auto ContextPool::borrowContext(int id) -> ::gui::Context *
    {
        cpp_freertos::LockGuard lock(mutex);
        const auto it = std::find(freeContextIds.begin(), freeContextIds.end(), id);
        if (it == freeContextIds.end()) {
            return nullptr;
        }

        lockContext(id);
        return contexts[id].get();
    }

    auto ContextPool::isAnyContextLocked() const -> bool
    {
        cpp_freertos::LockGuard lock(mutex);
        return !lockedContextIds.empty();
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

    void ContextPool::returnContext(int id)
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
