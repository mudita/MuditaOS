// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneNumberData.hpp"

namespace bluetooth
{
    PhoneNumberData::PhoneNumberData(const utils::PhoneNumber::View &view) : view(view)
    {}
    PhoneNumberData::PhoneNumberData(const utils::PhoneNumber &number) : view(number.getView())
    {}
    auto PhoneNumberData::getData() -> DataVariant
    {
        return view;
    }
} // namespace bluetooth
