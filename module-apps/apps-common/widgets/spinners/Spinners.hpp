// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ItemSpinner.hpp"

#include "Model.hpp"
#include "StringOutputSpinner.hpp"
#include "ItemSpinner.hpp"

namespace gui
{
    template <typename ValType, size_t Width>
    struct FixedIntegerFormatter
    {
        std::string operator()(const ValType val) const
        {
            std::stringstream outStream;
            outStream << std::setw(Width) << std::setfill('0') << val;
            return outStream.str();
        }
    };

    using StringContainer = Model<UTF8>;
    using UINT8Container  = Model<std::uint8_t>;
    using UINT16Container = Model<std::uint16_t>;

    using StringSpinner    = StringOutputSpinner<StringContainer>;
    using U8IntegerSpinner = StringOutputSpinner<UINT8Container>;

    using U8IntegerSpinnerFixed  = StringOutputSpinner<UINT8Container, FixedIntegerFormatter<std::uint32_t, 2>>;
    using U16IntegerSpinnerFixed = StringOutputSpinner<UINT16Container, FixedIntegerFormatter<std::uint32_t, 4>>;

    using WidgetSpinner = ItemSpinner<Model<Item *>>;
    template <typename T>
    using U8IntegerSpinnerWithFormatter = StringOutputSpinner<UINT8Container, T>;

} // namespace gui
