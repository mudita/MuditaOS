#include "EventDetailDescriptionItem.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "application-calendar/data/TimeDisplayParser.hpp"
#include <Style.hpp>
#include <time/time_conversion.hpp>
#include <module-utils/date/include/date/date.h>

namespace gui
{

    EventDetailDescriptionItem::EventDetailDescriptionItem()
    {
        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        setMinimumSize(style::window::default_body_width, style::window::calendar::item::eventDetail::height_min);
        setMaximumSize(style::window::default_body_width, style::window::calendar::item::eventDetail::height_max);
        setMargins(gui::Margins(style::margins::small, style::window::calendar::item::eventDetail::margin_top, 0, 0));

        vBox = new VBox(this, 0, 0, 0, 0);
        vBox->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        title = new gui::Label(vBox, 0, 0, 0, 0);
        title->setMinimumSize(style::window::default_body_width, style::window::calendar::item::eventDetail::title_h);
        title->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        title->setFont(style::window::font::small);
        title->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        title->setLineMode(true);
        title->activeItem = false;

        eventTime = new gui::Label(vBox, 0, 0, 0, 0);
        eventTime->setMinimumSize(style::window::default_body_width,
                                  style::window::calendar::item::eventDetail::label_h);
        eventTime->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        eventTime->setMargins(
            gui::Margins(0, style::window::calendar::item::eventDetail::event_time_margin, 0, style::margins::small));
        eventTime->setFont(style::window::font::bigbold);
        eventTime->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        eventTime->activeItem = false;

        description = new gui::Text(vBox, 0, 0, 0, 0);
        description->setMinimumSize(style::window::default_body_width,
                                    style::window::calendar::item::eventDetail::label_h);
        description->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        description->setMaximumSize(style::window::default_body_width,
                                    2 * style::window::calendar::item::eventDetail::label_h);
        description->setFont(style::window::font::big);
        description->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        description->setEditMode(EditMode::BROWSE);

        focusChangedCallback = [&](Item &item) {
            setFocusItem(focus ? vBox : nullptr);
            return true;
        };

        descriptionHandler();
    }

    bool EventDetailDescriptionItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        vBox->setPosition(0, 0);
        vBox->setSize(newDim.w, newDim.h);
        return true;
    }

    void EventDetailDescriptionItem::descriptionHandler()
    {
        title->setText(utils::localize.get("app_calendar_event_detail"));
        onLoadCallback = [&](std::shared_ptr<EventsRecord> event) {
            description->setText(event->title);
            eventTime->setText(TimeDisplayParser().getTimeString(event));
        };
    }

} /* namespace gui */
