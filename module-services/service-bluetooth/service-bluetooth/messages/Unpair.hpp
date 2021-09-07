// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "service-bluetooth/BluetoothMessage.hpp"

namespace message::bluetooth
{
    class Unpair : public BluetoothMessage
    {
      public:
        explicit Unpair(std::string addr) : addr(std::move(addr))
        {}
        [[nodiscard]] auto getAddr() const -> std::string
        {
            return addr;
        }

      private:
        std::string addr;
    };

    class UnpairResult : public BluetoothMessage
    {
      public:
        explicit UnpairResult(std::string addr, bool succeed) : addr(std::move(addr)), succeed(succeed)
        {}
        [[nodiscard]] auto getAddr() const -> std::string
        {
            return addr;
        }
        [[nodiscard]] auto isSucceed() const noexcept -> bool
        {
            return succeed;
        }

      private:
        std::string addr;
        bool succeed;
    };
} // namespace message::bluetooth
