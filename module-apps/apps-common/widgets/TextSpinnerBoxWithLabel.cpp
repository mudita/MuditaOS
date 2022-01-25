// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TextSpinnerBoxWithLabel.hpp"

namespace gui
{
    TextSpinnerBoxWithLabel::TextSpinnerBoxWithLabel(Item *parent,
                                                     const std::string &description,
                                                     const std::vector<UTF8> &data,
                                                     Boundaries boundaries)
        : VBox(parent)
    {
        setEdges(RectangleEdge::None);
        descriptionLabel = new gui::Label(this, 0, 0, 0, 0);
        descriptionLabel->setMinimumSize(style::window::default_body_width, style::text_spinner_label::label_h);
        descriptionLabel->setEdges(gui::RectangleEdge::None);
        descriptionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        descriptionLabel->setFont(style::window::font::small);
        descriptionLabel->activeItem = false;
        descriptionLabel->setText(description);

        optionSpinner = new gui::TextSpinnerBox(this, data, boundaries);
        optionSpinner->setMinimumSize(style::window::default_body_width, style::text_spinner_label::spinner_h);

        focusChangedCallback = [&](gui::Item &item) {
            setFocusItem(focus ? optionSpinner : nullptr);
            return true;
        };
    }

    void TextSpinnerBoxWithLabel::setData(const std::vector<UTF8> &data)
    {
        optionSpinner->setData(data);
    }

    UTF8 TextSpinnerBoxWithLabel::getCurrentValue() const noexcept
    {
        return optionSpinner->getCurrentValue();
    }

    void TextSpinnerBoxWithLabel::setCurrentValue(UTF8 val)
    {
        optionSpinner->setCurrentValue(val);
    }
} // namespace gui
