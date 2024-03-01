// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/AsyncTask.hpp>

namespace app
{
    class ApplicationCommon;
}

namespace gui
{
    template <class ValueType>
    class AbstractSettingsModel
    {
      public:
        virtual ~AbstractSettingsModel() = default;

        virtual void setValue(ValueType value) = 0;
        virtual ValueType getValue() const     = 0;
        virtual void restoreDefault()
        {}
    };
} // namespace gui
