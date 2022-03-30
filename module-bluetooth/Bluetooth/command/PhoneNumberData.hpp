// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "CommandData.hpp"

namespace bluetooth
{

    class PhoneNumberData : public CommandData
    {
      public:
        explicit PhoneNumberData(const utils::PhoneNumber::View &view);
        explicit PhoneNumberData(const utils::PhoneNumber &number);
        auto getData() -> DataVariant override;

      private:
        utils::PhoneNumber::View view;
    };

} // namespace bluetooth
