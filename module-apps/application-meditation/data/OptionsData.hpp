// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/SwitchData.hpp>
#include <functional>
#include <chrono>

namespace gui
{
    template <class T>
    class OptionParam
    {
      public:
        OptionParam(T value, std::function<void(T newValue)> callback) : value(value), onChangeCallback(callback)
        {}
        [[nodiscard]] T get() const noexcept
        {
            return value;
        }
        void set(T newValue)
        {
            value = newValue;
            if (onChangeCallback != nullptr) {
                onChangeCallback(newValue);
            }
        }

      private:
        T value;
        std::function<void(T newValue)> onChangeCallback{nullptr};
    };

    class OptionsData : public gui::SwitchData
    {
      public:
        struct OptionParams
        {
            OptionParam<std::chrono::seconds> preparationTime;
            OptionParam<bool> showCounter;
        };

        OptionsData(OptionParams params);

        [[nodiscard]] std::chrono::seconds getPreparationTime() const noexcept;
        bool setPreparationTime(std::chrono::seconds value);

        [[nodiscard]] bool isCounterVisible() const noexcept;
        void setCounterVisible(bool value);

      private:
        OptionParams params;
    };
} // namespace gui
