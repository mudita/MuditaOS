#pragma once

#include "gui/SwitchData.hpp"

#include <chrono>
namespace gui
{
    class OptionsData : public gui::SwitchData
    {
      public:
        OptionsData() = default;
        OptionsData(int preparationInSec, int preparationRecord, bool showCounter)
            : preparationTime(preparationInSec), preparationRecordIndex(preparationRecord), showCounter(showCounter)
        {}

        std::chrono::seconds preparationTime{5};
        uint32_t preparationRecordIndex = 0;
        bool showCounter                = true;
    };
} // namespace gui
