// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MultipleNumbersWidget.hpp"

#include <application-phonebook/data/PhonebookStyle.hpp>

#include <Label.hpp>

namespace gui
{
    MultipleNumbersWidget::MultipleNumbersWidget(const std::string &description, const std::string &number)
    {
        setMinimumSize(style::window::default_body_width, phonebookStyle::multipleNumbersWidget::height);
        setMargins(gui::Margins(0, style::margins::small, 0, style::margins::small));

        vBox = new gui::VBox(this);
        vBox->setEdges(gui::RectangleEdge::None);
        vBox->setAlignment({gui::Alignment::Vertical::Center});

        auto numberLabel = new gui::Label(vBox);
        numberLabel->setEdges(RectangleEdge::None);
        numberLabel->setFont(style::window::font::bigbold);
        numberLabel->setAlignment({gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top});
        numberLabel->setMargins({0, 0, 0, style::margins::big});
        numberLabel->setText(number);
        numberLabel->setMinimumWidthToFitText();
        numberLabel->setMinimumHeightToFitText();

        auto descriptionLabel = new gui::Label(vBox);
        descriptionLabel->setEdges(RectangleEdge::None);
        descriptionLabel->setFont(style::window::font::medium);
        descriptionLabel->setAlignment({gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top});
        descriptionLabel->setText(description);
        descriptionLabel->setMinimumWidthToFitText();
        descriptionLabel->setMinimumHeightToFitText();

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            vBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }
} /* namespace gui */
