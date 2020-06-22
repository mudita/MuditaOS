#include "InputLineWithLabelItem.hpp"

#include "application-phonebook/data/PhonebookStyle.hpp"

#include <module-utils/i18/i18.hpp>

namespace gui
{
    InputLineWithLabelItem::InputLineWithLabelItem(phonebookInternals::ListItemName listItemName) : listItemName(listItemName)
    {
        setMinimumSize(phonebookStyle::newContactItem::w, phonebookStyle::newContactItem::h);
        setMaximumSize(phonebookStyle::newContactItem::w, phonebookStyle::newContactItem::h);

        vBox = new gui::VBox(this, 0, 0, phonebookStyle::contactItem::w, phonebookStyle::contactItem::h);
        vBox->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);

        label = new gui::Label(this, 0, 0, vBox->getWidth(), phonebookStyle::newContactItem::title_h);
        label->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        label->setAlignment(Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_TOP));
        label->setFont(style::window::font::small);
        label->activeItem = false;

        text = new gui::Text(this, 0, 0, 0, 0);
        text->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        text->setAlignment(Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_BOTTOM));
        text->setFont(style::window::font::medium);
        text->setInputMode(new InputMode({InputMode::ABC, InputMode::abc, InputMode::digit}, [=](const UTF8 &) {}));
        text->setPenFocusWidth(2);
        text->setPenWidth(1);
        text->setEditMode(gui::Text::EditMode::EDIT);

        switch (listItemName) {
        case phonebookInternals::ListItemName::FirstName:
            label->setText(utils::localize.get("app_phonebook_new_contact_first_name"));
            text->setTextType(gui::Text::TextType::SINGLE_LINE);
            break;
        case phonebookInternals::ListItemName::SecondName:
            label->setText(utils::localize.get("app_phonebook_new_contact_second_name"));
            text->setTextType(gui::Text::TextType::SINGLE_LINE);
            break;
        case phonebookInternals::ListItemName::Number:
            label->setText(utils::localize.get("app_phonebook_new_contact_number_1"));
            text->setTextType(gui::Text::TextType::SINGLE_LINE);
            break;
        case phonebookInternals::ListItemName::OtherNumber:
            label->setText(utils::localize.get("app_phonebook_new_contact_number_2"));
            text->setTextType(gui::Text::TextType::SINGLE_LINE);
            break;
        case phonebookInternals::ListItemName::Email:
            label->setText(utils::localize.get("app_phonebook_new_contact_email"));
            text->setTextType(gui::Text::TextType::SINGLE_LINE);
            break;
        case phonebookInternals::ListItemName::Address:
            label->setText(utils::localize.get("app_phonebook_new_contact_address"));
            text->setTextType(gui::Text::TextType::MULTI_LINE);
            break;
        case phonebookInternals::ListItemName::Note:
            label->setText(utils::localize.get("app_phonebook_new_contact_note"));
            text->setTextType(gui::Text::TextType::MULTI_LINE);
            break;
        default:
            LOG_ERROR("Incorrect List Item Name!");
            break;
        }

        this->focusChangedCallback = [&](gui::Item &item) {
            if (this->focus) {
                setFocusItem(text);
            }
            else {
                setFocusItem(nullptr);
            }
            return true;
        };

        this->inputCallback = [&](gui::Item &item, const gui::InputEvent &event) { return text->onInput(event); };

        this->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        vBox->addWidget(label);
        vBox->addWidget(text);
    }

    InputLineWithLabelItem::InputLineWithLabeltem(const InputLineWithLabelItem &item) : InputLineWithLabelItem(item.listItemName)
    {}

    auto InputLineWithLabelItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool
    {
        vBox->setPosition(0, 0);
        vBox->setSize(newDim.w, newDim.h);

        label->setPosition(0, 0);
        label->setSize(newDim.w, phonebookStyle::newContactItem::title_h);

        text->setPosition(0, phonebookStyle::newContactItem::title_value_span);
        text->setSize(newDim.w, phonebookStyle::newContactItem::value_h);

        return true;
    }

    auto InputLineWithLabelItem::clone() -> gui::ListItem *
    {
        return new InputLineWithLabelItem(*this);
    }

} /* namespace gui */
