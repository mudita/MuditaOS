#include "InputLinesWithLabelIWidget.hpp"

#include <Span.hpp>
#include "application-phonebook/data/PhonebookStyle.hpp"

#include <ContactRecord.hpp>
#include <module-utils/i18/i18.hpp>
#include <utility>

namespace gui
{
    InputLinesWithLabelIWidget::InputLinesWithLabelIWidget(phonebookInternals::ListItemName listItemName,
                                                           std::function<void(const UTF8 &)> bottomBarTemporaryMode,
                                                           std::function<void()> bottomBarRestoreFromTemporaryMode,
                                                           std::function<void()> selectSpecialCharacter,
                                                           std::function<void()> contentChanged,
                                                           unsigned int lines)
        : listItemName(listItemName), checkTextContent(std::move(contentChanged))
    {
        setMinimumSize(phonebookStyle::inputLinesWithLabelWidget::w,
                       phonebookStyle::inputLinesWithLabelWidget::title_label_h +
                           phonebookStyle::inputLinesWithLabelWidget::span_size +
                           phonebookStyle::inputLinesWithLabelWidget::input_text_h * lines);
        setMargins(gui::Margins(0, style::margins::huge, 0, 0));

        vBox = new VBox(this, 0, 0, 0, 0);
        vBox->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        titleLabel = new Label(vBox);
        titleLabel->setMinimumSize(phonebookStyle::inputLinesWithLabelWidget::w,
                                   phonebookStyle::inputLinesWithLabelWidget::title_label_h);
        titleLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        titleLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        titleLabel->setFont(style::window::font::verysmall);
        titleLabel->activeItem = false;

        inputText = new TextFixedSize(vBox, 0, 0, 0, 0);
        inputText->setMinimumSize(phonebookStyle::inputLinesWithLabelWidget::w,
                                  phonebookStyle::inputLinesWithLabelWidget::input_text_h * lines);
        inputText->setMargins(Margins(0, phonebookStyle::inputLinesWithLabelWidget::span_size, 0, 0));
        inputText->setUnderlinePadding(phonebookStyle::inputLinesWithLabelWidget::underline_padding);

        inputText->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        inputText->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
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

        focusChangedCallback = [&](Item &item) {
            setFocusItem(focus ? vBox : nullptr);

            auto tempText = inputText->getText();

            if (focus) {
                inputText->setFont(style::window::font::mediumbold);
                inputText->setText(tempText);
            }
            else {
                inputText->setFont(style::window::font::medium);
                inputText->setText(tempText);
            }
            return true;
        };

        inputCallback = [&](Item &item, const InputEvent &event) {
            auto result = inputText->onInput(event);
            checkTextContent();
            return result;
        };
        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    }

    auto InputLinesWithLabelIWidget::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool
    {
        vBox->setPosition(0, 0);
        vBox->setSize(newDim.w, newDim.h);

        return true;
    }

    void InputLinesWithLabelIWidget::applyItemNameSpecificSettings()
    {
        switch (listItemName) {
        case phonebookInternals::ListItemName::FirstName:
            firstNameHandler();
            break;

        case phonebookInternals::ListItemName::SecondName:
            secondNameHandler();
            break;

        case phonebookInternals::ListItemName::Number:
            numberHandler();
            break;

        case phonebookInternals::ListItemName::SecondNumber:
            secondNumberHandler();
            break;

        case phonebookInternals::ListItemName::Email:
            emailHandler();
            break;

        case phonebookInternals::ListItemName::Address:
            addressHandler();
            break;

        case phonebookInternals::ListItemName::Note:
            noteHandler();
            break;

        default:
            LOG_ERROR("Incorrect List Item Name!");
            break;
        }
    }
    void InputLinesWithLabelIWidget::firstNameHandler()
    {
        titleLabel->setText(utils::localize.get("app_phonebook_new_contact_first_name"));
        inputText->setTextType(TextType::SINGLE_LINE);

        onSaveCallback = [&](std::shared_ptr<ContactRecord> contact) { contact->primaryName = inputText->getText(); };
        onLoadCallback = [&](std::shared_ptr<ContactRecord> contact) { inputText->setText(contact->primaryName); };
        onEmptyCallback = [&]() { return inputText->isEmpty(); };
    }
    void InputLinesWithLabelIWidget::secondNameHandler()
    {
        titleLabel->setText(utils::localize.get("app_phonebook_new_contact_second_name"));
        inputText->setTextType(TextType::SINGLE_LINE);

        onSaveCallback = [&](std::shared_ptr<ContactRecord> contact) {
            contact->alternativeName = inputText->getText();
        };
        onLoadCallback = [&](std::shared_ptr<ContactRecord> contact) { inputText->setText(contact->alternativeName); };
        onEmptyCallback = [&]() { return inputText->isEmpty(); };
    }
    void InputLinesWithLabelIWidget::numberHandler()
    {
        titleLabel->setText(utils::localize.get("app_phonebook_new_contact_number"));
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
        onEmptyCallback = [&]() { return inputText->isEmpty(); };
    }
    void InputLinesWithLabelIWidget::secondNumberHandler()
    {
        titleLabel->setText(utils::localize.get("app_phonebook_new_contact_second_number"));
        inputText->setTextType(TextType::SINGLE_LINE);
        inputText->setInputMode(new InputMode({InputMode::phone}));

        onSaveCallback = [&](std::shared_ptr<ContactRecord> contact) {
            if (inputText->getText().length() > 0) {
                contact->numbers.emplace_back(
                    ContactRecord::Number(utils::PhoneNumber(inputText->getText()).getView()));
            }
        };
        onLoadCallback = [&](std::shared_ptr<ContactRecord> contact) {
            if (contact->numbers.size() > 1) {
                inputText->setText(contact->numbers[1].number.getEntered());
            }
        };
        onEmptyCallback = [&]() { return inputText->isEmpty(); };
    }
    void InputLinesWithLabelIWidget::emailHandler()
    {
        titleLabel->setText(utils::localize.get("app_phonebook_new_contact_email"));
        inputText->setTextType(TextType::SINGLE_LINE);

        onSaveCallback = [&](std::shared_ptr<ContactRecord> contact) { contact->mail = inputText->getText(); };
        onLoadCallback = [&](std::shared_ptr<ContactRecord> contact) { inputText->setText(contact->mail); };
        onEmptyCallback = [&]() { return inputText->isEmpty(); };
    }
    void InputLinesWithLabelIWidget::addressHandler()
    {
        titleLabel->setText(utils::localize.get("app_phonebook_new_contact_address"));
        inputText->setTextType(TextType::SINGLE_LINE);

        onSaveCallback = [&](std::shared_ptr<ContactRecord> contact) { contact->address = inputText->getText(); };
        onLoadCallback = [&](std::shared_ptr<ContactRecord> contact) { inputText->setText(contact->address); };
    }
    void InputLinesWithLabelIWidget::noteHandler()
    {
        titleLabel->setText(utils::localize.get("app_phonebook_new_contact_note"));
        inputText->setTextType(TextType::SINGLE_LINE);

        onSaveCallback = [&](std::shared_ptr<ContactRecord> contact) { contact->note = inputText->getText(); };
        onLoadCallback = [&](std::shared_ptr<ContactRecord> contact) { inputText->setText(contact->note); };
    }

} /* namespace gui */
