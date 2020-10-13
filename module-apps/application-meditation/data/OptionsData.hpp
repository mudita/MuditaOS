#pragma once

#include "gui/SwitchData.hpp"

namespace gui
{
    class OptionsData : public gui::SwitchData
    {
      public:
        OptionsData() = default;
        OptionsData(int preparationInSec, int preparationRecord, bool showCounter)
            : preparationTimeInSeconds(preparationInSec), preparationRecordIndex(preparationRecord),
              showCounter(showCounter)
        {}

        int preparationTimeInSeconds    = 5;
        uint32_t preparationRecordIndex = 0;
        bool showCounter                = true;
    };
} // namespace gui
