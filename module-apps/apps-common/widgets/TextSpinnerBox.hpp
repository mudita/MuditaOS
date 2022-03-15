// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <widgets/spinners/Spinners.hpp>
#include <BoxLayout.hpp>
#include <ImageBox.hpp>

#include <vector>

namespace gui
{
    class TextSpinnerBox : public HBox
    {
      public:
        TextSpinnerBox(Item *parent, std::vector<UTF8> data, Boundaries boundaries);
        void setData(const std::vector<UTF8> &data);
        [[nodiscard]] UTF8 getCurrentValue() const noexcept;
        void setCurrentValue(UTF8 val);

      private:
        StringSpinner *spinner = nullptr;
        ImageBox *leftArrow    = nullptr;
        ImageBox *rightArrow   = nullptr;
    };
} // namespace gui
