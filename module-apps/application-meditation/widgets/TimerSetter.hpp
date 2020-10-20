#pragma once

#include "Circle.hpp"
#include "Rect.hpp"
#include "Text.hpp"
#include "Label.hpp"

#include <chrono>

namespace gui
{
    class TimerSetter : public Rect
    {
        class State
        {
            static constexpr int minimalValue          = 10;
            static constexpr int maximalValue          = 90;
            static constexpr int defaultIncrementValue = 15;

            bool resetValueOnNumeric = true;
            int timeInMinutes        = defaultIncrementValue;

          public:
            std::chrono::minutes getTime() const noexcept
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
        Label *timeLabel     = nullptr;
        Label *timeUnitLabel = nullptr;

        void build();

      public:
        TimerSetter(Item *parent, const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h);

        bool onFocus(bool isFocused) final;
        bool onInput(const InputEvent &inputEvent) final;
        [[nodiscard]] std::chrono::seconds getTime() noexcept;
    };

} // namespace gui
