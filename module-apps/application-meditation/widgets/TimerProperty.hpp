// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/widgets/Circle.hpp>
#include <gui/widgets/Label.hpp>
#include <gui/widgets/Rect.hpp>
#include <gui/widgets/BoxLayout.hpp>

#include <chrono>

namespace gui
{
    class TimerProperty : public Rect
    {
        class State
        {
            static constexpr int counterMaxDigits             = 2;
            static constexpr int minimalValue                 = 1;
            static constexpr int maximalValue                 = 99;
            static constexpr int defaultMeditationTime        = 15;
            static constexpr std::array<const int, 4> timeArr = {15, 30, 60, 90};

            bool resetValueOnNumeric = true;
            int timeInMinutes        = defaultMeditationTime;

          public:
            [[nodiscard]] std::chrono::minutes getTime() const noexcept
            {
                return std::chrono::minutes{timeInMinutes};
            }
            void checkBounds() noexcept;
            void putNumericValue(int digit) noexcept;
            void increment() noexcept;
            void decrement() noexcept;
            void onFocus() noexcept
            {
                resetValueOnNumeric = true;
                checkBounds();
            }
        } state;

        Circle *circle       = nullptr;
        VBox *centerBody     = nullptr;
        Label *timeLabel     = nullptr;
        Rect *divRect        = nullptr;
        Label *timeUnitLabel = nullptr;

        void build();
        void setMeditationTime();

      public:
        TimerProperty(Item *parent, const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h);

        bool onFocus(bool isFocused) final;
        bool onInput(const InputEvent &inputEvent) final;
        [[nodiscard]] std::chrono::minutes getTime() noexcept;
    };

} // namespace gui
