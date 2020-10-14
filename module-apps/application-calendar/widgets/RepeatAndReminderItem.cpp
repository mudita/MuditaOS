#include "RepeatAndReminderItem.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include <Style.hpp>
#include <Utils.hpp>
#include <module-apps/application-calendar/ApplicationCalendar.hpp>

namespace gui
{

    RepeatAndReminderItem::RepeatAndReminderItem(app::ApplicationCalendar *application) : app(application)
    {
        activeItem = false;
        setEdges(RectangleEdge::None);
        setMinimumSize(style::window::default_body_width, style::window::calendar::item::repeatAndReminder::height);

        hBox = new HBox(this, 0, 0, 0, 0);
        hBox->setEdges(RectangleEdge::None);

        repeatVBox = new VBox(hBox, 0, 0, 0, 0);
        repeatVBox->setMinimumSize(style::window::default_body_width / 2,
                                   style::window::calendar::item::repeatAndReminder::height);
        repeatVBox->setEdges(RectangleEdge::None);

        reminderVBox = new VBox(hBox, 0, 0, 0, 0);
        reminderVBox->setMinimumSize(style::window::default_body_width / 2,
                                     style::window::calendar::item::repeatAndReminder::height);
        reminderVBox->setEdges(RectangleEdge::None);

        repeatTitle = new gui::Label(repeatVBox, 0, 0, 0, 0);
        repeatTitle->setMinimumSize(style::window::calendar::item::repeatAndReminder::description_w,
                                    style::window::calendar::item::repeatAndReminder::title_label_h);
        repeatTitle->setMargins(
            gui::Margins(0, 0, 0, style::window::calendar::item::repeatAndReminder::title_label_margin));
        repeatTitle->setEdges(RectangleEdge::None);
        repeatTitle->setFont(style::window::font::small);
        repeatTitle->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        repeatTitle->setLineMode(true);
        repeatTitle->activeItem = false;

        repeat = new gui::Label(repeatVBox, 0, 0, 0, 0);
        repeat->setMinimumSize(style::window::calendar::item::repeatAndReminder::description_w,
                               style::window::calendar::item::repeatAndReminder::description_h);
        repeat->setEdges(RectangleEdge::None);
        repeat->setFont(style::window::font::medium);
        repeat->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        repeat->activeItem = false;

        reminderTitle = new gui::Label(reminderVBox, 0, 0, 0, 0);
        reminderTitle->setMinimumSize(style::window::default_body_width / 2,
                                      style::window::calendar::item::repeatAndReminder::title_label_h);
        reminderTitle->setMargins(
            gui::Margins(0, 0, 0, style::window::calendar::item::repeatAndReminder::title_label_margin));
        reminderTitle->setEdges(RectangleEdge::None);
        reminderTitle->setFont(style::window::font::small);
        reminderTitle->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        reminderTitle->setLineMode(true);
        reminderTitle->activeItem = false;

        reminder = new gui::Label(reminderVBox, 0, 0, 0, 0);
        reminder->setMinimumSize(style::window::default_body_width / 2,
                                 style::window::calendar::item::repeatAndReminder::description_h);
        reminder->setEdges(RectangleEdge::None);
        reminder->setFont(style::window::font::medium);
        reminder->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        reminder->activeItem = false;

        descriptionHandler();
    }

    bool RepeatAndReminderItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        hBox->setPosition(0, 0);
        hBox->setSize(newDim.w, newDim.h);
        return true;
    }

    void RepeatAndReminderItem::descriptionHandler()
    {
        repeatTitle->setText(utils::localize.get("app_calendar_event_detail_repeat"));
        reminderTitle->setText(utils::localize.get("app_calendar_event_detail_reminder"));
        onLoadCallback = [&](std::shared_ptr<EventsRecord> event) {
            if (event->repeat >= app::ApplicationCalendar::repeatOptions.size()) {
                repeat->setText("app_calendar_custom_repeat_title");
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
