// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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

    template <typename ValueT>
    class AsyncSettingsAdapter : public AbstractSettingsModel<ValueT>
    {
      public:
        std::function<void(ValueT)> onReady;

        void setValue(ValueT value) final
        {
            onSet(value);
        }

        ValueT getValue() const final
        {
            return currentValue;
        }

        void restoreDefault() final
        {
            onSet(defaultValue);
        }

      protected:
        std::function<void(ValueT)> onSet;

        void onUpdate(ValueT value)
        {
            currentValue = value;
            defaultValue = currentValue;
            onReady(currentValue);
        }

      private:
        ValueT currentValue{};
        ValueT defaultValue{};
    };
} // namespace gui
