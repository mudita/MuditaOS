#include "MultiLineTextWithLabelItem.hpp"

#include <Span.hpp>
#include "application-phonebook/data/PhonebookStyle.hpp"

#include <ContactRecord.hpp>
#include <module-utils/i18/i18.hpp>

namespace gui
{
    MultiLineTextWithLabelItem::MultiLineTextWithLabelItem(phonebookInternals::ListItemName listItemName)
        : listItemName(listItemName)
    {
        setMinimumSize(phonebookStyle::multiLineTextWithLabelItem::w, phonebookStyle::multiLineTextWithLabelItem::h);
        setMaximumSize(phonebookStyle::multiLineTextWithLabelItem::w, phonebookStyle::multiLineTextWithLabelItem::h);
        setMargins(gui::Margins(0, style::margins::very_big, 0, 0));

        vBox = new VBox(this, 0, 0, 0, phonebookStyle::inputLineWithLabelItem::title_label_h);
        vBox->setEdges(RectangleEdgeFlags::GUI_RECT_ALL_EDGES);

        titleLabel = new Label(vBox);
        titleLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        titleLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        titleLabel->setFont(style::window::font::small);
        titleLabel->setLineMode(true);
        titleLabel->activeItem = false;

        new gui::Span(
            vBox, Axis::Y, phonebookStyle::multiLineTextWithLabelItem::span_size); // spread title & multilineText

        multilineText = new Text(vBox, 0, 0, 0, phonebookStyle::multiLineTextWithLabelItem::input_text_h);
        multilineText->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        multilineText->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));
        multilineText->setFont(style::window::font::medium);
        multilineText->setPenFocusWidth(style::window::default_border_focus_w);
        multilineText->setPenWidth(style::window::default_border_no_focus_w);
        multilineText->setEditMode(EditMode::BROWSE);

        applyItemNameSpecificSettings();

        focusChangedCallback = [&](Item &item) {
            setFocusItem(focus ? vBox : nullptr);
            return true;
        };

        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    }

    auto MultiLineTextWithLabelItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool
    {
        vBox->setPosition(0, 0);
        vBox->setSize(newDim.w, newDim.h);

        // this shouldn't be needed - without it there will be nothing shown in place of digit labels
        titleLabel->setSize(newDim.w, phonebookStyle::multiLineTextWithLabelItem::title_label_h);
        multilineText->setSize(newDim.w, phonebookStyle::multiLineTextWithLabelItem::input_text_h);
        return true;
    }

    void MultiLineTextWithLabelItem::applyItemNameSpecificSettings()
    {
        switch (listItemName) {
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

    void MultiLineTextWithLabelItem::addressHandler()
    {
        titleLabel->setText(utils::localize.get("app_phonebook_new_contact_address"));

        onLoadCallback = [&](std::shared_ptr<ContactRecord> contact) { multilineText->setText(contact->address); };
    }
    void MultiLineTextWithLabelItem::noteHandler()
    {
        titleLabel->setText(utils::localize.get("app_phonebook_new_contact_note"));

        onLoadCallback = [&](std::shared_ptr<ContactRecord> contact) { multilineText->setText(contact->note); };
    }

} /* namespace gui */
