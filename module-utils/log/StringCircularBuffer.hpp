// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <string>
#include <optional>

class StringCircularBuffer
{
  public:
    explicit StringCircularBuffer(size_t size) : buffer(std::make_unique<std::string[]>(size)), capacity(size)
    {}
    [[nodiscard]] size_t getCapacity() const noexcept
    {
        return capacity;
    }
    [[nodiscard]] bool isEmpty() const noexcept
    {
        return size == 0;
    }
    [[nodiscard]] virtual std::optional<std::string> get();
    [[nodiscard]] size_t getSize() const noexcept
    {
        return size;
    }
    [[nodiscard]] bool isFull() const noexcept
    {
        return full;
    }
    void put(const std::string &item);
    void put(std::string &&item);
    void reset();

  private:
    void updateMembersAfterPut();
    void updateMembersBeforePut();

    std::unique_ptr<std::string[]> buffer;
    bool full{false};
    size_t head{0};
    size_t capacity;
    size_t size{0};
    size_t tail{0};
};
