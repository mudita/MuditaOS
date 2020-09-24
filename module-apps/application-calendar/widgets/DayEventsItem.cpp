#include "DayEventsItem.hpp"
#include "CalendarStyle.hpp"
#include "application-calendar/data/TimeDisplayParser.hpp"
#include <ListView.hpp>
#include <gui/widgets/Label.hpp>
#include <Style.hpp>

namespace gui
{

    DayEventsItem::DayEventsItem()
    {
        setMinimumSize(style::window::default_body_width, style::window::calendar::item::dayEvents::height);
        setMargins(gui::Margins(0, style::margins::small, 0, 0));

        vBox = new gui::VBox(this, 0, 0, 0, 0);
        vBox->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        hBox = new gui::HBox(vBox, 0, 0, 0, 0);
        hBox->setMinimumSize(style::window::default_body_width, style::window::calendar::item::dayEvents::box_height);
        hBox->setMargins(gui::Margins(0, style::margins::small + style::margins::big, 0, 0));
        hBox->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        title = new gui::Label(hBox, 0, 0, 0, 0);
        title->setMinimumSize(style::window::calendar::item::dayEvents::title_w,
                              style::window::calendar::item::dayEvents::box_height);
        title->setMargins(gui::Margins(style::margins::small, 0, style::margins::small + style::margins::big, 0));
        title->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        title->setFont(style::window::font::bigbold);
        title->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});

        clock = new gui::Image(hBox, 0, 0, 0, 0, "small_tick_W_M");
        clock->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});

        description = new gui::Label(vBox, 0, 0, 0, 0);
        description->setMinimumSize(style::window::default_body_width,
                                    style::window::calendar::item::dayEvents::box_height);
        description->setMargins(gui::Margins(style::margins::small, style::margins::big, 0, style::margins::very_big));
        description->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        description->setFont(style::window::font::medium);
        description->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
    }

    void DayEventsItem::setEvent(std::shared_ptr<EventsRecord> rec)
    {
        this->record = rec;

        if (rec != nullptr) {
            description->setText(this->record->title.c_str());
            title->setText(TimeDisplayParser().getTimeString(record));
            if (record->reminder == static_cast<uint32_t>(Reminder::never)) {
                clock->setVisible(false);
            }
        }
    }

    bool DayEventsItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        vBox->setPosition(0, 0);
        vBox->setSize(newDim.w, newDim.h);
        return true;
    }

} /* namespace gui */
