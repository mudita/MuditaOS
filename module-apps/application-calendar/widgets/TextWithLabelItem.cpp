#include "TextWithLabelItem.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include <Style.hpp>

namespace gui
{

    TextWithLabelItem::TextWithLabelItem(const std::string &description,
                                         std::function<void(const UTF8 &)> bottomBarTemporaryMode,
                                         std::function<void()> bottomBarRestoreFromTemporaryMode,
                                         std::function<void()> selectSpecialCharacter)
    {
        setMinimumSize(style::window::default_body_width, style::window::calendar::item::textWithLabel::height);
        setMargins(gui::Margins(style::margins::small, 0, 0, 0));
        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        bottomBarRestoreFromTemporaryMode();

        vBox = new gui::VBox(this, 0, 0, 0, 0);
        vBox->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        vBox->setPenFocusWidth(style::window::default_border_focus_w);
        vBox->setPenWidth(style::window::default_border_rect_no_focus);

        descriptionLabel = new gui::Label(vBox, 0, 0, 0, 0);
        descriptionLabel->setMinimumSize(style::window::default_body_width,
                                         style::window::calendar::item::textWithLabel::description_h);
        descriptionLabel->setMargins(gui::Margins(0, style::margins::small, 0, 0));
        descriptionLabel->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        descriptionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        descriptionLabel->setFont(style::window::font::small);
        descriptionLabel->activeItem = false;
        descriptionLabel->setText(description);

        textInput = new gui::Text(vBox, 0, 0, 0, 0);
        textInput->setMinimumSize(style::window::default_body_width,
                                  style::window::calendar::item::textWithLabel::text_input_h);
        textInput->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        textInput->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));
        textInput->setFont(style::window::font::medium);
        textInput->setInputMode(new InputMode(
            {InputMode::ABC, InputMode::abc, InputMode::digit},
            [=](const UTF8 &text) { bottomBarTemporaryMode(text); },
            [=]() { bottomBarRestoreFromTemporaryMode(); },
            [=]() { selectSpecialCharacter(); }));
        textInput->setPenFocusWidth(style::window::default_border_focus_w);
        textInput->setPenWidth(style::window::default_border_rect_no_focus);
        textInput->setEditMode(gui::EditMode::EDIT);

        focusChangedCallback = [&](Item &item) {
            setFocusItem(focus ? vBox : nullptr);
            return true;
        };

        inputCallback = [&](Item &item, const InputEvent &event) {
            auto ret       = textInput->onInput(event);
            onSaveCallback = [&](std::shared_ptr<EventsRecord> record) { record->title = textInput->getText(); };
            return ret;
        };

        onLoadCallback = [&](std::shared_ptr<EventsRecord> event) { textInput->setText(event->title); };
    }

    bool TextWithLabelItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        vBox->setPosition(0, 0);
        vBox->setSize(newDim.w, newDim.h);
        return true;
    }

} /* namespace gui */
