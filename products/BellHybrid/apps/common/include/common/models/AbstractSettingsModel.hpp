// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace gui
{
    template <class ValueType> class AbstractSettingsModel
    {
      public:
        virtual ~AbstractSettingsModel() = default;

        virtual void setValue(ValueType value) = 0;
        virtual ValueType getValue() const     = 0;
    };
} // namespace gui
