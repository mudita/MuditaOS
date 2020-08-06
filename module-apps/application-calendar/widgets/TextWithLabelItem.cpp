#include "TextWithLabelItem.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "windows/AppWindow.hpp"
#include <Style.hpp>

namespace gui
{

    TextWithLabelItem::TextWithLabelItem(const std::string &description)
    {
        setMinimumSize(style::window::default_body_width, style::window::calendar::item::checkBox::height);
        setMaximumSize(style::window::default_body_width, style::window::calendar::item::checkBox::height);

        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        vBox = new gui::VBox(this, 0, 0, style::window::default_body_width, 0);
        vBox->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        vBox->setPenFocusWidth(style::window::default_border_focus_w);
        vBox->setPenWidth(style::window::default_border_rect_no_focus);

        descriptionLabel = new gui::Label(vBox, 0, 0, 0, 0);
        descriptionLabel->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        descriptionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        descriptionLabel->setFont(style::window::font::medium);
        descriptionLabel->activeItem = false;
        descriptionLabel->setText(description);

        textInput = new gui::Text(vBox, 0, 0, style::window::default_body_width, 50);
        textInput->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        textInput->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));
        textInput->setFont(style::window::font::medium);
        textInput->setInputMode(new InputMode({InputMode::ABC, InputMode::abc, InputMode::digit}));
        textInput->setPenFocusWidth(style::window::default_border_focus_w);
        textInput->setPenWidth(style::window::default_border_rect_no_focus);
        textInput->setEditMode(gui::EditMode::EDIT);

        focusChangedCallback = [&](Item &item) {
            setFocusItem(focus ? vBox : nullptr);
            return true;
        };

        inputCallback = [&](Item &item, const InputEvent &event) { return textInput->onInput(event); };
    }

    bool TextWithLabelItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        vBox->setPosition(0, 0);
        vBox->setSize(newDim.w, newDim.h);
        LOG_DEBUG("SIZE: %i,  %i", newDim.w, newDim.h);

        descriptionLabel->setArea(BoundingBox(style::window::calendar::item::checkBox::description_label_x,
                                              newDim.h - style::window::calendar::item::checkBox::description_label_h,
                                              style::window::calendar::item::checkBox::description_label_w,
                                              style::window::calendar::item::checkBox::description_label_h));

        return true;
    }

} /* namespace gui */
