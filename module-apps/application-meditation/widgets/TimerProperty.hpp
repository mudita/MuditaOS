// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/widgets/Circle.hpp>
#include <gui/widgets/text/Label.hpp>
#include <gui/widgets/Rect.hpp>
#include <gui/widgets/BoxLayout.hpp>

#include <chrono>

namespace gui
{
    class TimerProperty : public Rect
    {
        class State
        {
            static constexpr int counterMaxDigits               = 2;
            static constexpr std::array<const int, 4> timeArray = {15, 30, 60, 90};

            bool resetValueOnNumeric = true;
            int timeInMinutes;

          public:
            [[nodiscard]] std::chrono::minutes getTime() const noexcept
            {
                return std::chrono::minutes{timeInMinutes};
            }
            bool setTime(int value);
            void checkBounds() noexcept;
            void putNumericValue(int digit) noexcept;
            void delNumericValue() noexcept;
            void increment() noexcept;
            void decrement() noexcept;
            void onFocus() noexcept
            {
                resetValueOnNumeric = true;
                checkBounds();
            }
        } state;

      public:
        using OnChangeCallback = std::function<void(std::int32_t newValue)>;

        TimerProperty(
            Item *parent, const std::uint32_t x, const std::uint32_t y, const std::uint32_t w, const std::uint32_t h);

        bool onFocus(bool isFocused) final;
        bool onInput(const InputEvent &inputEvent) final;
        [[nodiscard]] std::chrono::minutes getTime() noexcept;
        bool setTime(std::int32_t newValue);
        void setOnChangeCallback(OnChangeCallback callback);

      private:
        Circle *circle       = nullptr;
        VBox *centerBody     = nullptr;
        Label *timeLabel     = nullptr;
        Rect *divRect        = nullptr;
        Label *timeUnitLabel = nullptr;
        OnChangeCallback onChangeCallback;

        void build();
        void setMeditationTime();
    };

} // namespace gui
