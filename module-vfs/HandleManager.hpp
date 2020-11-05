// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <cstdint>
#include <array>
#include <atomic>
namespace vfsn::internal::stdlib
{
    // Generic handle manager standard syscalls
    namespace impl
    {
        class HandleManager
        {
            static constexpr auto MAX_HANDLES  = 128;
            static constexpr auto FIRST_HANDLE = 3;

          protected:
            auto clearHandle(int hwnd) noexcept -> void *;
            auto setHandle(void *hwnd) noexcept -> int;
            auto get(int handle) const noexcept -> void *
            {
                if (handle < FIRST_HANDLE || handle >= FIRST_HANDLE + MAX_HANDLES)
                    return nullptr;
                else
                    return reinterpret_cast<void *>(mHandles[handle - FIRST_HANDLE].load());
            }

          private:
            std::array<std::atomic<void *>, MAX_HANDLES> mHandles;
        };
    } // namespace impl
    template <typename T> class HandleManager : public impl::HandleManager
    {
      public:
        auto setHandle(T *hwnd) noexcept
        {
            return impl::HandleManager::setHandle(hwnd);
        }
        auto operator[](int handle) const noexcept
        {
            return reinterpret_cast<T *>(impl::HandleManager::get(handle));
        }
        auto clearHandle(int hwnd)
        {
            return reinterpret_cast<T *>(impl::HandleManager::clearHandle(hwnd));
        }
    };
} // namespace vfsn::internal::stdlib
