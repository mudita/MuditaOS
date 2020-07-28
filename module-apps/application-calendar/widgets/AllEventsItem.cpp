#include "AllEventsItem.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include <Style.hpp>
#include <gui/widgets/Label.hpp>
#include <time/time_conversion.hpp>

namespace gui
{
    AllEventsItem::AllEventsItem()
    {
        setMinimumSize(style::window::default_body_width, style::window::label::big_h);
        setMaximumSize(style::window::default_body_width, style::window::label::big_h);

        startTime = new gui::Label(this, 0, 0, 0, 0);
        startTime->setPenFocusWidth(0);
        startTime->setPenWidth(0);
        startTime->setFont(style::window::font::small);
        startTime->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});

        description = new gui::Label(this, 0, 0, 0, 0);
        description->setPenFocusWidth(0);
        description->setPenWidth(0);
        description->setFont(style::window::font::bigbold);
        description->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});

        description->setText(utils::localize.get("common_information"));
        startTime->setText(utils::time::DateTime().str());
    }

    bool AllEventsItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        startTime->setPosition(style::window::calendar::item::all::start_time_x, 0);
        startTime->setSize(style::window::default_body_width, style::window::label::big_h);

        description->setPosition(style::window::calendar::item::all::description_x, 0);
        description->setSize(style::window::calendar::item::all::description_w, style::window::label::big_h);

        return true;
    }

    void AllEventsItem::setMarkerItem(UTF8 text)
    {
        description->setText("");
        startTime->setText(text);
        startTime->setLineMode(true);
        activeItem = false;
        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    }

    UTF8 AllEventsItem::getLabelMarker() const
    {
        return utils::time::Time().str("%d %B");
    }

} /* namespace gui */
