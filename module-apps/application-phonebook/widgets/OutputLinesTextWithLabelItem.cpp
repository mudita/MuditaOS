#include "OutputLinesTextWithLabelItem.hpp"

#include <Span.hpp>
#include "application-phonebook/data/PhonebookStyle.hpp"

#include <ContactRecord.hpp>
#include <module-utils/i18/i18.hpp>

namespace gui
{
    OutputLinesTextWithLabelItem::OutputLinesTextWithLabelItem(phonebookInternals::ListItemName listItemName)
        : listItemName(listItemName)
    {
        setMinimumSize(phonebookStyle::outputLinesTextWithLabelItem::w,
                       phonebookStyle::outputLinesTextWithLabelItem::title_label_h);
        setMargins(gui::Margins(0, style::margins::huge, 0, 0));

        vBox = new VBox(this, 0, 0, 0, 0);
        vBox->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        titleLabel = new Label(vBox);
        titleLabel->setMinimumSize(phonebookStyle::outputLinesTextWithLabelItem::w,
                                   phonebookStyle::outputLinesTextWithLabelItem::title_label_h);
        titleLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        titleLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        titleLabel->setFont(style::window::font::verysmall);
        titleLabel->setLineMode(true);
        titleLabel->activeItem = false;

        multilineText = new Text(vBox, 0, 0, 0, 0);
        multilineText->setMaximumSize(phonebookStyle::outputLinesTextWithLabelItem::w,
                                      phonebookStyle::outputLinesTextWithLabelItem::input_text_h * 10);
        multilineText->setMargins(Margins(0, phonebookStyle::outputLinesTextWithLabelItem::span_size, 0, 0));
        multilineText->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        multilineText->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));
        multilineText->setFont(style::window::font::medium);
        multilineText->setEditMode(EditMode::BROWSE);

        applyItemNameSpecificSettings();

        focusChangedCallback = [&](Item &item) {
            setFocusItem(focus ? vBox : nullptr);
            return true;
        };

        this->activeItem = false;
        setEdges(RectangleEdgeFlags::GUI_RECT_ALL_EDGES);
    }

    auto OutputLinesTextWithLabelItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool
    {
        vBox->setPosition(0, 0);
        vBox->setSize(newDim.w, newDim.h);

        return true;
    }

    void OutputLinesTextWithLabelItem::applyItemNameSpecificSettings()
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

    void OutputLinesTextWithLabelItem::addressHandler()
    {
        titleLabel->setText(utils::localize.get("app_phonebook_new_contact_address"));

        onLoadCallback = [&](std::shared_ptr<ContactRecord> contact) { multilineText->setText(contact->address); };
    }

    void OutputLinesTextWithLabelItem::noteHandler()
    {
        titleLabel->setText(utils::localize.get("app_phonebook_new_contact_note"));

        onLoadCallback = [&](std::shared_ptr<ContactRecord> contact) { multilineText->setText(contact->note); };
    }

    auto OutputLinesTextWithLabelItem::handleRequestResize(const Item *child,
                                                           unsigned short request_w,
                                                           unsigned short request_h) -> Size
    {

        setMinimumHeight(phonebookStyle::outputLinesTextWithLabelItem::title_label_h +
                         phonebookStyle::outputLinesTextWithLabelItem::span_size + request_h);

        return Size(request_w, request_h);
    }

} /* namespace gui */
