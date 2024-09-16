// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

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

        virtual auto setValue(ValueType value) -> void = 0;
        virtual auto getValue() const -> ValueType     = 0;
        virtual auto restoreDefault() -> void
        {}
    };
} // namespace gui
