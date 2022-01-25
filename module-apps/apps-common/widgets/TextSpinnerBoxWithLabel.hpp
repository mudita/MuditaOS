// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Label.hpp>
#include <BoxLayout.hpp>
#include <widgets/TextSpinnerBox.hpp>

namespace style::text_spinner_label
{
    inline constexpr auto spinner_h = 40;
    inline constexpr auto label_h   = 27;
    inline constexpr auto h         = spinner_h + label_h;
} // namespace style::text_spinner_label

namespace gui
{
    class TextSpinnerBoxWithLabel : public VBox
    {
      public:
        TextSpinnerBoxWithLabel(Item *parent,
                                const std::string &description,
                                const std::vector<UTF8> &data,
                                Boundaries boundaries);

        void setData(const std::vector<UTF8> &data);
        [[nodiscard]] UTF8 getCurrentValue() const noexcept;
        void setCurrentValue(UTF8 val);

      protected:
        gui::TextSpinnerBox *optionSpinner = nullptr;

      private:
        gui::Label *descriptionLabel = nullptr;
    };
} // namespace gui
