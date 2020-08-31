#include "DayEventsItem.hpp"
#include <ListView.hpp>
#include <gui/widgets/Label.hpp>
#include <Style.hpp>
#include <time/time_conversion.hpp>

namespace gui
{

    DayEventsItem::DayEventsItem()
    {
        setMinimumSize(style::window::default_body_width, style::window::calendar::item::dayEvents::height);

        vBox = new gui::VBox(this, 0, 0, 0, 0);
        vBox->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        hBox = new gui::HBox(vBox, 0, 0, 0, 0);
        hBox->setMinimumSize(style::window::default_body_width, style::window::calendar::item::dayEvents::h_box_h);
        hBox->setMargins(gui::Margins(0, style::window::calendar::item::dayEvents::margins, 0, 0));
        hBox->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        title = new gui::Label(hBox, 0, 0, 0, 0);
        title->setMinimumSize(style::window::calendar::item::dayEvents::title_w,
                              style::window::calendar::item::dayEvents::h_box_h);
        title->setMargins(gui::Margins(0, 0, style::window::calendar::item::dayEvents::margins, 0));
        title->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        title->setFont(style::window::font::bigbold);
        title->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});

        clock = new gui::Image("small_tick");
        clock->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        hBox->addWidget(clock);

        description = new gui::Label(vBox, 0, 0, 0, 0);
        description->setMinimumSize(style::window::default_body_width,
                                    style::window::calendar::item::dayEvents::description_h);
        description->setMargins(gui::Margins(0, 0, 0, style::window::calendar::item::dayEvents::margins));
        description->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        description->setFont(style::window::font::medium);
        description->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
    }

    void DayEventsItem::setEvent(std::shared_ptr<EventsRecord> rec)
    {
        this->record = rec;

        if (rec != nullptr) {
            description->setText(this->record->title.c_str());
            uint32_t start_time = this->record->date_from % 10000;
            uint32_t end_time   = this->record->date_till % 10000;
            std::string text    = std::to_string(start_time) + " - " + std::to_string(end_time);
            title->setText(text);
        }
    }

    bool DayEventsItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        vBox->setPosition(0, 0);
        vBox->setSize(newDim.w, newDim.h);
        return true;
    }

} /* namespace gui */
