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
        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        setMinimumSize(style::window::default_body_width, style::window::calendar::item::repeatAndReminder::height);

        hBox = new HBox(this, 0, 0, 0, 0);
        hBox->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        repeatVBox = new VBox(hBox, 0, 0, 0, 0);
        repeatVBox->setMinimumSize(style::window::default_body_width / 2,
                                   style::window::calendar::item::repeatAndReminder::height);
        repeatVBox->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        reminderVBox = new VBox(hBox, 0, 0, 0, 0);
        reminderVBox->setMinimumSize(style::window::default_body_width / 2,
                                     style::window::calendar::item::repeatAndReminder::height);
        reminderVBox->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        repeatTitle = new gui::Label(repeatVBox, 0, 0, 0, 0);
        repeatTitle->setMinimumSize(style::window::calendar::item::repeatAndReminder::description_w,
                                    style::window::calendar::item::repeatAndReminder::title_label_h);
        repeatTitle->setMargins(
            gui::Margins(0, 0, 0, style::window::calendar::item::repeatAndReminder::title_label_margin));
        repeatTitle->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        repeatTitle->setFont(style::window::font::small);
        repeatTitle->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        repeatTitle->setLineMode(true);
        repeatTitle->activeItem = false;

        repeat = new gui::Label(repeatVBox, 0, 0, 0, 0);
        repeat->setMinimumSize(style::window::calendar::item::repeatAndReminder::description_w,
                               style::window::calendar::item::repeatAndReminder::description_h);
        repeat->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        repeat->setFont(style::window::font::medium);
        repeat->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        repeat->activeItem = false;

        reminderTitle = new gui::Label(reminderVBox, 0, 0, 0, 0);
        reminderTitle->setMinimumSize(style::window::default_body_width / 2,
                                      style::window::calendar::item::repeatAndReminder::title_label_h);
        reminderTitle->setMargins(
            gui::Margins(0, 0, 0, style::window::calendar::item::repeatAndReminder::title_label_margin));
        reminderTitle->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        reminderTitle->setFont(style::window::font::small);
        reminderTitle->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        reminderTitle->setLineMode(true);
        reminderTitle->activeItem = false;

        reminder = new gui::Label(reminderVBox, 0, 0, 0, 0);
        reminder->setMinimumSize(style::window::default_body_width / 2,
                                 style::window::calendar::item::repeatAndReminder::description_h);
        reminder->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
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
