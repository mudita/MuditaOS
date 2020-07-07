#include "InputLineWithLabelItem.hpp"

#include "application-phonebook/data/PhonebookStyle.hpp"

#include <module-utils/i18/i18.hpp>

namespace gui
{
    InputLineWithLabelItem::InputLineWithLabelItem(phonebookInternals::ListItemName listItemName,
                                                   std::function<void(const UTF8 &)> bottomBarTemporaryMode,
                                                   std::function<void()> bottomBarRestoreFromTemporaryMode,
                                                   std::function<void()> selectSpecialCharacter)
        : listItemName(listItemName)
    {
        setMinimumSize(phonebookStyle::inputLineWithLabelItem::w, phonebookStyle::inputLineWithLabelItem::h);
        setMaximumSize(phonebookStyle::inputLineWithLabelItem::w, phonebookStyle::inputLineWithLabelItem::h);

        vBox = new VBox(this, 0, 0, 0, 0);
        vBox->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);

        titleLabel = new Label(vBox, 0, 0, 0, 0);
        titleLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        titleLabel->setAlignment(Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_TOP));
        titleLabel->setFont(style::window::font::small);
        titleLabel->activeItem = false;

        inputText = new Text(vBox, 0, 0, 0, 0);
        inputText->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        inputText->setAlignment(Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_BOTTOM));
        inputText->setFont(style::window::font::medium);
        inputText->setInputMode(new InputMode(
            {InputMode::ABC, InputMode::abc, InputMode::digit},
            [=](const UTF8 &text) { bottomBarTemporaryMode(text); },
            [=]() { bottomBarRestoreFromTemporaryMode(); },
            [=]() { selectSpecialCharacter(); }));
        inputText->setPenFocusWidth(style::window::default_border_focus_w);
        inputText->setPenWidth(style::window::default_border_no_focus_w);
        inputText->setEditMode(EditMode::EDIT);

        applyItemNameSpecificSettings();

        this->focusChangedCallback = [&](Item &item) {
            if (this->focus) {
                setFocusItem(inputText);
            }
            else {
                setFocusItem(nullptr);
            }
            return true;
        };

        this->inputCallback = [&](Item &item, const InputEvent &event) { return inputText->onInput(event); };
        this->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        this->deleteByList = deleteByList;
    }

    InputLineWithLabelItem::InputLineWithLabelItem(const InputLineWithLabelItem &item)
        : InputLineWithLabelItem(item.listItemName)
    {}

    auto InputLineWithLabelItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool
    {
        vBox->setPosition(0, 0);
        vBox->setSize(newDim.w, newDim.h);

        titleLabel->setPosition(0, 0);
        titleLabel->setSize(newDim.w, phonebookStyle::inputLineWithLabelItem::title_label_h);

        inputText->setPosition(0, phonebookStyle::inputLineWithLabelItem::title_label_input_text_span);
        inputText->setSize(newDim.w, phonebookStyle::inputLineWithLabelItem::input_text_h);

        return true;
    }

    void InputLineWithLabelItem::applyItemNameSpecificSettings()
    {
        switch (listItemName) {
        case phonebookInternals::ListItemName::FirstName:
            titleLabel->setText(utils::localize.get("app_phonebook_new_contact_first_name"));
            inputText->setTextType(TextType::SINGLE_LINE);
            break;
        case phonebookInternals::ListItemName::SecondName:
            titleLabel->setText(utils::localize.get("app_phonebook_new_contact_second_name"));
            inputText->setTextType(TextType::SINGLE_LINE);
            break;
        case phonebookInternals::ListItemName::Number:
            titleLabel->setText(utils::localize.get("app_phonebook_new_contact_number_1"));
            inputText->setTextType(TextType::SINGLE_LINE);
            inputText->setInputMode(new InputMode({InputMode::phone}));
            break;
        case phonebookInternals::ListItemName::OtherNumber:
            titleLabel->setText(utils::localize.get("app_phonebook_new_contact_number_2"));
            inputText->setTextType(TextType::SINGLE_LINE);
            break;
        case phonebookInternals::ListItemName::Email:
            titleLabel->setText(utils::localize.get("app_phonebook_new_contact_email"));
            inputText->setTextType(TextType::SINGLE_LINE);
            break;
        case phonebookInternals::ListItemName::Address:
            titleLabel->setText(utils::localize.get("app_phonebook_new_contact_address"));
            inputText->setTextType(TextType::MULTI_LINE);
            break;
        case phonebookInternals::ListItemName::Note:
            titleLabel->setText(utils::localize.get("app_phonebook_new_contact_note"));
            inputText->setTextType(TextType::MULTI_LINE);
            break;
        default:
            LOG_ERROR("Incorrect List Item Name!");
            break;
        }
    }
} /* namespace gui */
