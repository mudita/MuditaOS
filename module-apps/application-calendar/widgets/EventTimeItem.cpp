#include "application-calendar/models/CustomRepeatModel.hpp"
#include "EventTimeItem.hpp"
#include <ListView.hpp>
#include <Style.hpp>
#include <time/time_conversion.hpp>

namespace gui
{

    EventTimeItem::EventTimeItem(const std::string &description,
                                 std::function<void(const UTF8 &)> bottomBarTemporaryMode,
                                 std::function<void()> bottomBarRestoreFromTemporaryMode)
        : bottomBarTemporaryMode(std::move(bottomBarTemporaryMode)),
          bottomBarRestoreFromTemporaryMode(std::move(bottomBarRestoreFromTemporaryMode))
    {
        setMinimumSize(style::window::default_body_width, 80);
        setMaximumSize(style::window::default_body_width, 80);

        // setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        hBox = new gui::HBox(this, 0, 0, 450, 0);
        hBox->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        hBox->setPenFocusWidth(style::window::default_border_focus_w);
        hBox->setPenWidth(1);

        colonLabel = new gui::Label(hBox, 0, 0, 0, 0);
        colonLabel->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        colonLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));
        colonLabel->setFont(style::window::font::medium);
        colonLabel->setText(":");
        colonLabel->activeItem = false;

        descriptionLabel = new gui::Label(hBox, 0, 0, 0, 0);
        descriptionLabel->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        descriptionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        descriptionLabel->setFont(style::window::font::medium);
        descriptionLabel->activeItem = false;

        hourInput = new gui::Text(hBox, 0, 0, 0, 0);
        hourInput->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        hourInput->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));
        hourInput->setFont(style::window::font::medium);
        hourInput->setInputMode(new InputMode({InputMode::digit}));
        hourInput->setPenFocusWidth(style::window::default_border_focus_w);
        hourInput->setPenWidth(1);
        hourInput->setEditMode(gui::EditMode::EDIT);

        minuteInput = new gui::Text(hBox, 0, 0, 0, 0);
        minuteInput->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        minuteInput->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));
        minuteInput->setFont(style::window::font::medium);
        minuteInput->setInputMode(new InputMode({InputMode::digit}));
        minuteInput->setPenFocusWidth(style::window::default_border_focus_w);
        minuteInput->setPenWidth(1);
        minuteInput->setEditMode(gui::EditMode::EDIT);

        mode12hInput = new gui::Text(hBox, 0, 0, 0, 0);
        mode12hInput->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        mode12hInput->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));
        mode12hInput->setFont(style::window::font::medium);
        mode12hInput->setInputMode(new InputMode({InputMode::ABC}));
        mode12hInput->setPenFocusWidth(style::window::default_border_focus_w);
        mode12hInput->setPenWidth(1);
        mode12hInput->setEditMode(gui::EditMode::EDIT);

        descriptionLabel->setText(description);
    }

    bool EventTimeItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        hBox->setPosition(0, 0);
        hBox->setSize(newDim.w, newDim.h);
        LOG_DEBUG("SIZE TIME ITEM: %i,  %i", newDim.w, newDim.h);

        descriptionLabel->setArea(BoundingBox(15, 0, 150, 20));
        colonLabel->setArea(BoundingBox(50, 30, 10, 20));
        hourInput->setArea(BoundingBox(15, 20, 140, newDim.h - 20));
        minuteInput->setArea(BoundingBox(175, 20, 140, newDim.h - 20));
        mode12hInput->setArea(BoundingBox(335, 20, 140, newDim.h - 20));

        return true;
    }

    bool EventTimeItem::onActivated(void *data)
    {
        return true;
    }

} /* namespace gui */
