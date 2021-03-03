// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RepeatAndReminderItem.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "application-calendar/ApplicationCalendar.hpp"
#include "application-alarm-clock/data/AlarmsData.hpp"
#include <Style.hpp>
#include <Utils.hpp>
#include <Image.hpp>

namespace gui
{

    RepeatAndReminderItem::RepeatAndReminderItem(app::ApplicationCalendar *application) : app(application)
    {
        activeItem = false;
        setEdges(RectangleEdge::None);
        setMinimumSize(style::window::default_body_width, style::window::calendar::item::repeatAndReminder::height);
        setMargins(gui::Margins(style::margins::small, 0, 0, 0));

        std::vector<gui::Image *> squareImages;
        for (int i = 0; i < 2; i++) {
            auto square = new gui::Image();
            square->set("Rectangle");
            square->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
            squareImages.push_back(square);
        }

        hBox = new HBox(this, 0, 0, 0, 0);
        hBox->setEdges(RectangleEdge::None);

        repeatVBox = new VBox(hBox, 0, 0, 0, 0);
        repeatVBox->setMinimumSize(style::window::default_body_width / 2 - style::margins::small / 2,
                                   style::window::calendar::item::repeatAndReminder::height);
        repeatVBox->setEdges(RectangleEdge::None);

        reminderVBox = new VBox(hBox, 0, 0, 0, 0);
        reminderVBox->setMinimumSize(style::window::default_body_width / 2 - style::margins::small / 2,
                                     style::window::calendar::item::repeatAndReminder::height);
        reminderVBox->setEdges(RectangleEdge::None);

        repeatHBox = new HBox(repeatVBox, 0, 0, 0, 0);
        repeatHBox->setMinimumSize(style::window::default_body_width / 2 - style::margins::small / 2,
                                   style::window::calendar::item::repeatAndReminder::title_label_h);
        repeatHBox->setMargins(
            gui::Margins(0, 0, 0, style::window::calendar::item::repeatAndReminder::title_label_margin));
        repeatHBox->setEdges(RectangleEdge::None);
        repeatHBox->addWidget(squareImages[0]);

        repeatTitle = new gui::Label(repeatHBox, 0, 0, 0, 0);
        repeatTitle->setMinimumSize(style::window::calendar::item::repeatAndReminder::description_w,
                                    style::window::calendar::item::repeatAndReminder::title_label_h);
        repeatTitle->setMargins(gui::Margins(style::margins::small, 0, 0, 0));
        repeatTitle->setEdges(RectangleEdge::None);
        repeatTitle->setFont(style::window::font::small);
        repeatTitle->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        repeatTitle->activeItem = false;

        repeat = new gui::Label(repeatVBox, 0, 0, 0, 0);
        repeat->setMinimumSize(style::window::calendar::item::repeatAndReminder::description_w,
                               style::window::calendar::item::repeatAndReminder::description_h);
        repeat->setEdges(RectangleEdge::None);
        repeat->setFont(style::window::font::medium);
        repeat->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        repeat->activeItem = false;

        reminderHBox = new HBox(reminderVBox, 0, 0, 0, 0);
        reminderHBox->setMinimumSize(style::window::default_body_width / 2 - style::margins::small / 2,
                                     style::window::calendar::item::repeatAndReminder::title_label_h);
        reminderHBox->setMargins(
            gui::Margins(0, 0, 0, style::window::calendar::item::repeatAndReminder::title_label_margin));
        reminderHBox->setEdges(RectangleEdge::None);
        reminderHBox->addWidget(squareImages[1]);

        reminderTitle = new gui::Label(reminderHBox, 0, 0, 0, 0);
        reminderTitle->setMinimumSize(style::window::calendar::item::repeatAndReminder::description_w,
                                      style::window::calendar::item::repeatAndReminder::title_label_h);
        reminderTitle->setMargins(gui::Margins(style::margins::small, 0, 0, 0));
        reminderTitle->setEdges(RectangleEdge::None);
        reminderTitle->setFont(style::window::font::small);
        reminderTitle->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        reminderTitle->activeItem = false;

        reminder = new gui::Label(reminderVBox, 0, 0, 0, 0);
        reminder->setMinimumSize(style::window::default_body_width / 2,
                                 style::window::calendar::item::repeatAndReminder::description_h);
        reminder->setEdges(RectangleEdge::None);
        reminder->setFont(style::window::font::medium);
        reminder->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        reminder->activeItem = false;

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            hBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };

        descriptionHandler();
    }

    void RepeatAndReminderItem::descriptionHandler()
    {
        repeatTitle->setText(utils::localize.get("app_calendar_event_detail_repeat"));
        reminderTitle->setText(utils::localize.get("app_calendar_event_detail_reminder"));
        onLoadCallback = [&](std::shared_ptr<EventsRecord> event) {
            if (event->repeat > app::ApplicationCalendar::repeatOptions.size()) {
                repeat->setText(CustomRepeatValueParser(event->repeat).getWeekDaysText());
            }
            else {
                repeat->setText(utils::localize.get(
                    app::ApplicationCalendar::repeatOptions.at(static_cast<Repeat>(event->repeat))));
            }
            reminder->setText(utils::localize.get(
                app::ApplicationCalendar::reminderOptions.at(static_cast<Reminder>(event->reminder))));
        };
    }

} /* namespace gui */
