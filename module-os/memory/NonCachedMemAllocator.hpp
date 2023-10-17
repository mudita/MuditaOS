// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <cassert>

#include <FreeRTOS.h>

template <typename T> struct NonCachedMemAllocator
{
    using value_type = T;

    T *allocate(std::size_t num);
    void deallocate(T *ptr, std::size_t num);

    NonCachedMemAllocator()                              = default;
    NonCachedMemAllocator(const NonCachedMemAllocator &) = default;
    NonCachedMemAllocator(NonCachedMemAllocator &&)      = default;

    NonCachedMemAllocator &operator=(const NonCachedMemAllocator &) = default;
    NonCachedMemAllocator &operator=(NonCachedMemAllocator &&) = default;
};

template <typename T> T *NonCachedMemAllocator<T>::allocate(std::size_t num)
{
    T *ptr = reinterpret_cast<T *>(pvPortMalloc(sizeof(T) * num));
    return ptr;
}

template <typename T> void NonCachedMemAllocator<T>::deallocate(T *ptr, std::size_t)
{
    assert(ptr != nullptr);
    vPortFree(ptr);
}
