// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OutputLinesTextWithLabelWidget.hpp"

#include <Span.hpp>
#include "application-phonebook/data/PhonebookStyle.hpp"

#include <ContactRecord.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    OutputLinesTextWithLabelWidget::OutputLinesTextWithLabelWidget(phonebookInternals::ListItemName listItemName)
        : listItemName(listItemName)
    {
        setMinimumSize(phonebookStyle::outputLinesTextWithLabelWidget::w,
                       phonebookStyle::outputLinesTextWithLabelWidget::title_label_h);
        setMargins(gui::Margins(style::widgets::leftMargin, style::margins::huge, 0, 0));

        vBox = new VBox(this, 0, 0, 0, 0);
        vBox->setEdges(RectangleEdge::None);

        titleLabel = new TextWithSnippet(vBox, style::window::font::verysmall);
        titleLabel->setMinimumSize(phonebookStyle::outputLinesTextWithLabelWidget::w,
                                   phonebookStyle::outputLinesTextWithLabelWidget::title_label_h);
        titleLabel->setEdges(RectangleEdge::None);
        titleLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        titleLabel->activeItem = false;

        multilineText = new Text(vBox, 0, 0, 0, 0);
        multilineText->setMaximumSize(phonebookStyle::outputLinesTextWithLabelWidget::w,
                                      phonebookStyle::outputLinesTextWithLabelWidget::input_text_h * 10);
        multilineText->setMargins(Margins(0, phonebookStyle::outputLinesTextWithLabelWidget::span_size, 0, 0));
        multilineText->setEdges(RectangleEdge::None);
        multilineText->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));
        multilineText->setFont(style::window::font::medium);
        multilineText->setEditMode(EditMode::Browse);

        applyItemNameSpecificSettings();

        focusChangedCallback = [&](Item &item) {
            setFocusItem(focus ? vBox : nullptr);
            return true;
        };

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            vBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };

        this->activeItem = false;
        setEdges(RectangleEdge::All);
    }

    void OutputLinesTextWithLabelWidget::applyItemNameSpecificSettings()
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

    void OutputLinesTextWithLabelWidget::addressHandler()
    {
        titleLabel->textWidget->setText(utils::translate("app_phonebook_new_contact_address"));

        onLoadCallback = [&](std::shared_ptr<ContactRecord> contact) { multilineText->setText(contact->address); };
    }

    void OutputLinesTextWithLabelWidget::noteHandler()
    {
        titleLabel->textWidget->setText(utils::translate("app_phonebook_new_contact_note"));

        onLoadCallback = [&](std::shared_ptr<ContactRecord> contact) { multilineText->setText(contact->note); };
    }

    auto OutputLinesTextWithLabelWidget::handleRequestResize(const Item *child, Length request_w, Length request_h)
        -> Size
    {
        setMinimumHeight(phonebookStyle::outputLinesTextWithLabelWidget::title_label_h +
                         phonebookStyle::outputLinesTextWithLabelWidget::span_size + request_h);

        return Size(request_w, request_h);
    }

} /* namespace gui */
