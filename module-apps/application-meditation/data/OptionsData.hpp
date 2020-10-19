#pragma once

#include "gui/SwitchData.hpp"

#include <chrono>
namespace gui
{
    class OptionsData : public gui::SwitchData
    {
      public:
        OptionsData() = default;
        OptionsData(std::chrono::seconds preparation, bool showCounter)
            : preparationTime(preparation), showCounter(showCounter)
        {}

        std::chrono::seconds preparationTime{5};
        bool showCounter = true;
    };
} // namespace gui
