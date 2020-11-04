// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "HandleManager.hpp"

namespace vfsn::internal::stdlib::impl
{
    auto HandleManager::clearHandle(int hwnd) noexcept -> void *
    {
        if (hwnd < FIRST_HANDLE || hwnd >= MAX_HANDLES + FIRST_HANDLE) {
            return nullptr;
        }
        hwnd -= FIRST_HANDLE;
        return mHandles[hwnd].exchange(nullptr);
    }

    auto HandleManager::setHandle(void *hwnd) noexcept -> int
    {
        int h;
        for (h = 0; h < MAX_HANDLES; ++h) {
            void *tst_val = nullptr;
            if (mHandles[h].compare_exchange_strong(tst_val, hwnd)) {
                return h + FIRST_HANDLE;
            }
        }
        return -1;
    }
} // namespace vfs::internal::stdlib::impl
