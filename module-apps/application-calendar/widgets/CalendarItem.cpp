#include "../models/CalendarModel.hpp"
#include "ListView.hpp"
#include "gui/widgets/Label.hpp"

#include "CalendarItem.hpp"
#include <Style.hpp>

namespace gui
{

    CalendarItem::CalendarItem(CalendarModel *model) : model{model}
    {
        setMinimumSize(style::window::default_body_width, model->getMinimalItemHeight());
        setMaximumSize(style::window::default_body_width, model->getMinimalItemHeight());

        setRadius(0);
        setPenFocusWidth(2);
        setPenWidth(0);

        title = new gui::Label(this, 0, 0, 0, 0);
        title->setPenFocusWidth(0);
        title->setPenWidth(0);
        title->setFont(style::window::font::bigbold);
        title->setAlignment(
            gui::Alignment{gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_CENTER});

        description = new gui::Label(this, 0, 0, 0, 0);
        description->setPenFocusWidth(0);
        description->setPenWidth(0);
        description->setFont(style::window::font::medium);
        description->setAlignment(
            gui::Alignment{gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_CENTER});

        // clock = new gui::Image(this, 0, 0, 0, 0, "menu_alarm_W_G"); // get clock image

        // temp
        description->setText("Meeting");
        title->setText("10:00 - 11:15 AM");
    }

    bool CalendarItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        title->setPosition(11, 10);
        title->setSize(250, 40);

        description->setPosition(11, 42);
        description->setSize(newDim.w - 22, newDim.h - 40);

        return true;
    }

    bool CalendarItem::onActivated(void *data)
    {
        LOG_DEBUG("Open calendar item");
        return true;
    }

} /* namespace gui */
