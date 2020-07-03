#include "InputLineWithLabelItem.hpp"

#include "application-phonebook/data/PhonebookStyle.hpp"

#include <module-db/Interface/ContactRecord.hpp>
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
        inputText->setAlignment(Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_CENTER));
        inputText->setFont(style::window::font::big);
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
    }

    auto InputLineWithLabelItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool
    {
        vBox->setPosition(0, 0);
        vBox->setSize(newDim.w, newDim.h);

        titleLabel->setArea(BoundingBox(0, 0, newDim.w, phonebookStyle::inputLineWithLabelItem::title_label_h));

        inputText->setArea(BoundingBox(0,
                                       phonebookStyle::inputLineWithLabelItem::title_label_input_text_span,
                                       newDim.w,
                                       phonebookStyle::inputLineWithLabelItem::input_text_h));

        return true;
    }

    void InputLineWithLabelItem::applyItemNameSpecificSettings()
    {
        switch (listItemName) {
        case phonebookInternals::ListItemName::FirstName:
            titleLabel->setText(utils::localize.get("app_phonebook_new_contact_first_name"));
            inputText->setFont(style::window::font::bigbold);
            inputText->setTextType(Text::TextType::SINGLE_LINE);

            onSaveCallback = [&](std::shared_ptr<ContactRecord> contact) {
                contact->primaryName = inputText->getText();
            };
            onLoadCallback = [&](std::shared_ptr<ContactRecord> contact) { inputText->setText(contact->primaryName); };
            break;

        case phonebookInternals::ListItemName::SecondName:
            titleLabel->setText(utils::localize.get("app_phonebook_new_contact_second_name"));
            inputText->setTextType(Text::TextType::SINGLE_LINE);

            onSaveCallback = [&](std::shared_ptr<ContactRecord> contact) {
                contact->alternativeName = inputText->getText();
            };
            onLoadCallback = [&](std::shared_ptr<ContactRecord> contact) {
                inputText->setText(contact->alternativeName);
            };
            break;

        case phonebookInternals::ListItemName::Number:
            titleLabel->setText(utils::localize.get("app_phonebook_new_contact_number_1"));
            inputText->setTextType(TextType::SINGLE_LINE);
            inputText->setInputMode(new InputMode({InputMode::phone}));

            onSaveCallback = [&](std::shared_ptr<ContactRecord> contact) {
                if (inputText->getText().length() > 0) {
                    contact->numbers.emplace_back(
                        ContactRecord::Number(utils::PhoneNumber(inputText->getText()).getView()));
                }
            };
            onLoadCallback = [&](std::shared_ptr<ContactRecord> contact) {
                if (contact->numbers.size() > 0) {
                    inputText->setText(contact->numbers[0].number.getEntered());
                }
            };
            break;

        case phonebookInternals::ListItemName::OtherNumber:
            titleLabel->setText(utils::localize.get("app_phonebook_new_contact_number_2"));
            inputText->setTextType(Text::TextType::SINGLE_LINE);
            inputText->setInputMode(new InputMode({InputMode::phone}));

            onSaveCallback = [&](std::shared_ptr<ContactRecord> contact) {
                if (inputText->getText().length() > 0) {
                    // Temporary disable saving secondary number since multiple numbers are not supported yet, and this
                    // could lead to confusing errors
                    // contact->numbers.emplace_back(ContactRecord::Number(utils::PhoneNumber(inputText->getText()).getView()));
                }
            };
            onLoadCallback = [&](std::shared_ptr<ContactRecord> contact) {
                if (contact->numbers.size() > 1) {
                    inputText->setText(contact->numbers[1].number.getEntered());
                }
            };
            break;

        case phonebookInternals::ListItemName::Email:
            titleLabel->setText(utils::localize.get("app_phonebook_new_contact_email"));
            inputText->setTextType(Text::TextType::SINGLE_LINE);

            onSaveCallback = [&](std::shared_ptr<ContactRecord> contact) { contact->mail = inputText->getText(); };
            onLoadCallback = [&](std::shared_ptr<ContactRecord> contact) { inputText->setText(contact->mail); };
            break;

        case phonebookInternals::ListItemName::Address:
            titleLabel->setText(utils::localize.get("app_phonebook_new_contact_address"));
            inputText->setTextType(Text::TextType::SINGLE_LINE);

            onSaveCallback = [&](std::shared_ptr<ContactRecord> contact) { contact->address = inputText->getText(); };
            onLoadCallback = [&](std::shared_ptr<ContactRecord> contact) { inputText->setText(contact->address); };
            break;

        case phonebookInternals::ListItemName::Note:
            titleLabel->setText(utils::localize.get("app_phonebook_new_contact_note"));
            inputText->setTextType(Text::TextType::SINGLE_LINE);

            onSaveCallback = [&](std::shared_ptr<ContactRecord> contact) { contact->note = inputText->getText(); };
            onLoadCallback = [&](std::shared_ptr<ContactRecord> contact) { inputText->setText(contact->note); };
            break;

        default:
            LOG_ERROR("Incorrect List Item Name!");
            break;
        }
    }

} /* namespace gui */
