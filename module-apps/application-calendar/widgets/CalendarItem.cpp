#include "../models/CalendarModel.hpp"
#include "ListView.hpp"
#include "gui/widgets/Label.hpp"

#include "CalendarItem.hpp"
#include <Style.hpp>
#include <memory>

namespace gui
{

    CalendarItem::CalendarItem(CalendarModel *model) : model{model}
    {
        setMinimumSize(style::window::default_body_width, 90);
        setMaximumSize(style::window::default_body_width, 90);

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

        //clock = new gui::Image(this, 0, 0, 0, 0, "menu_alarm_W_G"); // get clock image

        /// TODO: Change to load text from database
        description->setText("Meeting");
        title->setText("10:00 - 11:15 AM");
    }

    CalendarItem::~CalendarItem()
    {

    }

    bool CalendarItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        title->setPosition(11, 10);
        title->setSize(250, 40);

        description->setPosition(11, 42);
        description->setSize(newDim.w - 22, newDim.h - 40);

        return true;
    }

    /// TODO: Load text to labels from database
    /*void CalendarItem::setNote(std::shared_ptr< > &note)
    {
        this->note = note;
        // set values of the labels
        //title->setText(std::to_string(note->ID));
        //description->setText(note->path);
        title->setText(note.title);
        description->setText(note.note);
    }*/

    bool CalendarItem::onActivated(void *data)
    {
        LOG_INFO("OPEN ITEM");
        return true;
    }

} /* namespace gui */