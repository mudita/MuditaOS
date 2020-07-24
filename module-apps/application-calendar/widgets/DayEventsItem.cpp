#include "DayEventsItem.hpp"
#include <ListView.hpp>
#include <gui/widgets/Label.hpp>
#include <Style.hpp>
#include <time/time_conversion.hpp>

namespace gui
{

    DayEventsItem::DayEventsItem()
    {
        setMinimumSize(style::window::default_body_width, style::window::calendar::item::height);
        setMaximumSize(style::window::default_body_width, style::window::calendar::item::height);

        setPenFocusWidth(style::window::default_border_focus_w);
        setPenWidth(style::window::default_border_no_focus_w);

        title = new gui::Label(this, 0, 0, 0, 0);
        title->setPenFocusWidth(0);
        title->setPenWidth(0);
        title->setFont(style::window::font::bigbold);
        title->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});

        description = new gui::Label(this, 0, 0, 0, 0);
        description->setPenFocusWidth(0);
        description->setPenWidth(0);
        description->setFont(style::window::font::medium);
        description->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
    }

    void CalendarItem::setEvent(std::shared_ptr<EventsRecord> rec)
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
        title->setPosition(style::window::calendar::item::label_x, style::window::calendar::item::title_y);
        title->setSize(style::window::calendar::item::title_w, style::window::calendar::item::title_h);

        description->setPosition(style::window::calendar::item::label_x, style::window::calendar::item::descripton_y);
        description->setSize(newDim.w - 2 * style::window::calendar::item::label_x,
                             newDim.h - style::window::calendar::item::title_h);

        return true;
    }

} /* namespace gui */
