// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CheckBoxWithLabelItem.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "windows/AppWindow.hpp"
#include <ListView.hpp>
#include <Style.hpp>
#include <Utils.hpp>
#include <InputEvent.hpp>

namespace gui
{

    CheckBoxWithLabelItem::CheckBoxWithLabelItem(app::Application *application,
                                                 const std::string &description,
                                                 std::shared_ptr<WeekDaysRepeatData> data)
        : app(application), checkBoxData(std::move(data))
    {
        app = application;
        assert(app != nullptr);

        setMinimumSize(style::window::default_body_width, style::window::calendar::item::checkBox::height);
        setMargins(gui::Margins(
            style::window::calendar::leftMargin, style::window::calendar::item::checkBox::margin_top, 0, 0));
        setEdges(RectangleEdge::None);

        hBox = new gui::HBox(this, 0, 0, 0, 0);
        hBox->setEdges(gui::RectangleEdge::None);

        checkBox = new gui::CheckBox(
            hBox,
            0,
            0,
            0,
            0,
            [=](const UTF8 &text) {
                app->getCurrentWindow()->bottomBarTemporaryMode(text, BottomBar::Side::LEFT, false);
            },
            [=]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });

        checkBox->setMinimumSize(style::window::calendar::item::checkBox::input_box_label_w,
                                 style::window::calendar::item::checkBox::height);
        checkBox->activeItem = false;

        descriptionLabel = new gui::Label(hBox, 0, 0, 0, 0);
        descriptionLabel->setMinimumSize(style::window::calendar::item::checkBox::description_label_w,
                                         style::window::calendar::item::checkBox::height);
        descriptionLabel->setMargins(gui::Margins(style::margins::very_big, 0, 0, 0));
        descriptionLabel->setEdges(gui::RectangleEdge::None);
        descriptionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        descriptionLabel->setFont(style::window::font::medium);
        descriptionLabel->activeItem = false;
        descriptionLabel->setText(description);

        applyCallbacks();
        if (checkBoxData != nullptr) {
            setCheckBoxes();
        }
        onContentChangedCallback = [&]() { return checkBox->isChecked(); };
    }

    void CheckBoxWithLabelItem::applyCallbacks()
    {
        focusChangedCallback = [&](Item &item) {
            if (focus) {
                descriptionLabel->setFont(style::window::font::mediumbold);
                setFocusItem(checkBox);
            }
            else {
                descriptionLabel->setFont(style::window::font::medium);
            }
            return true;
        };

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (checkBox->onInput(event)) {
                checkBox->resizeItems();
                onContentChangedCallback = [&]() { return checkBox->isChecked(); };
                return true;
            }
            return false;
        };

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            hBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }

    void CheckBoxWithLabelItem::setCheckBoxes()
    {
        if (descriptionLabel->getText() == utils::localize.get(style::strings::common::Monday)) {
            checkBox->setImageVisible(checkBoxData->getData(date::Monday.iso_encoding() - 1));
        }
        else if (descriptionLabel->getText() == utils::localize.get(style::strings::common::Tuesday)) {
            checkBox->setImageVisible(checkBoxData->getData(date::Tuesday.iso_encoding() - 1));
        }
        else if (descriptionLabel->getText() == utils::localize.get(style::strings::common::Wednesday)) {
            checkBox->setImageVisible(checkBoxData->getData(date::Wednesday.iso_encoding() - 1));
        }
        else if (descriptionLabel->getText() == utils::localize.get(style::strings::common::Thursday)) {
            checkBox->setImageVisible(checkBoxData->getData(date::Thursday.iso_encoding() - 1));
        }
        else if (descriptionLabel->getText() == utils::localize.get(style::strings::common::Friday)) {
            checkBox->setImageVisible(checkBoxData->getData(date::Friday.iso_encoding() - 1));
        }
        else if (descriptionLabel->getText() == utils::localize.get(style::strings::common::Saturday)) {
            checkBox->setImageVisible(checkBoxData->getData(date::Saturday.iso_encoding() - 1));
        }
        else if (descriptionLabel->getText() == utils::localize.get(style::strings::common::Sunday)) {
            checkBox->setImageVisible(checkBoxData->getData(date::Sunday.iso_encoding() - 1));
        }
    }

} /* namespace gui */
