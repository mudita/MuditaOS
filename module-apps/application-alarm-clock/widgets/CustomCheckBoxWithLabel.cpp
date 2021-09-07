// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CustomCheckBoxWithLabel.hpp"
#include "AlarmClockStyle.hpp"
#include <InputEvent.hpp>

namespace gui
{
    const std::map<WeekDayIso, std::string> CustomCheckBoxWithLabel::weekDays = {
        {WeekDayIso::Monday, style::strings::common::Monday},
        {WeekDayIso::Tuesday, style::strings::common::Tuesday},
        {WeekDayIso::Wednesday, style::strings::common::Wednesday},
        {WeekDayIso::Thursday, style::strings::common::Thursday},
        {WeekDayIso::Friday, style::strings::common::Friday},
        {WeekDayIso::Saturday, style::strings::common::Saturday},
        {WeekDayIso::Sunday, style::strings::common::Sunday}};

    CustomCheckBoxWithLabel::CustomCheckBoxWithLabel(app::Application *app,
                                                     const std::string &description,
                                                     const WeekDaysRepeatData &data)
        : application(app), checkBoxData(data)
    {
        assert(application != nullptr);

        setMinimumSize(style::window::default_body_width, style::alarmClock::window::item::checkBox::height);
        setMargins(gui::Margins(style::margins::small, style::alarmClock::window::item::checkBox::marginTop, 0, 0));
        setEdges(RectangleEdge::None);

        hBox = new gui::HBox(this, 0, 0, 0, 0);
        hBox->setEdges(gui::RectangleEdge::None);

        checkBox = new gui::CheckBox(
            hBox,
            0,
            0,
            0,
            0,
            [=](const UTF8 &text) { application->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
            [=]() { application->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });
        checkBox->setMinimumSize(style::alarmClock::window::item::checkBox::inputBox_w,
                                 style::alarmClock::window::item::checkBox::height);

        descriptionLabel = new gui::Label(hBox, 0, 0, 0, 0);
        descriptionLabel->setMinimumSize(style::alarmClock::window::item::checkBox::description_w,
                                         style::alarmClock::window::item::checkBox::height);
        descriptionLabel->setMargins(gui::Margins(style::margins::very_big, 0, 0, 0));
        descriptionLabel->setEdges(gui::RectangleEdge::None);
        descriptionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        descriptionLabel->setFont(style::window::font::medium);
        descriptionLabel->setText(description);

        applyCallbacks();
        setCheckBoxes();
    }

    void CustomCheckBoxWithLabel::applyCallbacks()
    {
        focusChangedCallback = [&](Item &item) {
            if (focus) {
                descriptionLabel->setFont(style::window::font::mediumbold);
                setFocusItem(checkBox);
            }
            else {
                descriptionLabel->setFont(style::window::font::medium);
                setFocusItem(nullptr);
            }
            return true;
        };

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (event.is(gui::KeyCode::KEY_RF) || event.is(gui::KeyCode::KEY_ENTER)) {
                setFocusItem(nullptr);
                return false;
            }
            if (checkBox->onInput(event)) {
                checkBox->resizeItems();
                return true;
            }
            return false;
        };
        onContentChangedCallback = [&]() { return checkBox->isChecked(); };

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            hBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }

    void CustomCheckBoxWithLabel::setCheckBoxes()
    {
        for (auto const &[key, dayName] : weekDays) {
            if (descriptionLabel->getText() == utils::translate(dayName)) {
                checkBox->setCheck(checkBoxData.getData(static_cast<uint32_t>(key)));
            }
        }
    }

} // namespace gui
