#include "InformationWidget.hpp"

#include "application-phonebook/data/PhonebookStyle.hpp"
#include "NumberWithIconsWidget.hpp"

#include <ContactRecord.hpp>
#include <module-utils/i18/i18.hpp>

namespace gui
{
    InformationWidget::InformationWidget(app::Application *app)
    {
        setMargins(gui::Margins(0, style::margins::very_big, 0, 0));

        setMinimumSize(phonebookStyle::informationWidget::w, phonebookStyle::informationWidget::h);
        setMaximumSize(phonebookStyle::informationWidget::w, phonebookStyle::informationWidget::h);

        vBox = new VBox(this, 0, 0, 0, 0);
        vBox->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        titleLabel = new Label(vBox,
                               0,
                               0,
                               phonebookStyle::informationWidget::w,
                               phonebookStyle::informationWidget::title_label_h,
                               utils::localize.get("app_phonebook_contact_information"));
        titleLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        titleLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        titleLabel->setFont(style::window::font::small);
        titleLabel->setLineMode(true);
        titleLabel->activeItem = false;

        onLoadCallback = [=](std::shared_ptr<ContactRecord> contact) {
            if (contact->numbers.size() > 0) {

                setMinimumHeight(widgetMinimumArea.h + phonebookStyle::informationWidget::title_label_h);

                primaryNumberHBox = new NumberWithIconsWidget(app,
                                                              contact->numbers[0].number,
                                                              nullptr,
                                                              0,
                                                              0,
                                                              phonebookStyle::informationWidget::w,
                                                              phonebookStyle::informationWidget::h);
                vBox->addWidget(primaryNumberHBox);
            }
            if (contact->numbers.size() > 1) {
                setMinimumHeight(widgetMinimumArea.h + phonebookStyle::informationWidget::h);
                alternativeNumberHBox = new NumberWithIconsWidget(app,
                                                                  contact->numbers[1].number,
                                                                  nullptr,
                                                                  0,
                                                                  0,
                                                                  phonebookStyle::informationWidget::w,
                                                                  phonebookStyle::informationWidget::h);

                vBox->addWidget(alternativeNumberHBox);
            }
            if (contact->mail.length() > 0) {
                setMinimumHeight(widgetMinimumArea.h + phonebookStyle::informationWidget::h);

                emailText = new Text(nullptr, 0, 0, 0, 0);
                emailText->setMaximumSize(phonebookStyle::informationWidget::w,
                                          phonebookStyle::informationWidget::email_text_h);
                emailText->setFont(style::window::font::medium);
                emailText->setPenFocusWidth(style::window::default_border_focus_w);
                emailText->setPenWidth(style::window::default_border_no_focus_w);
                emailText->setEditMode(EditMode::BROWSE);
                emailText->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
                emailText->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
                emailText->setText(contact->mail);
                vBox->addWidget(emailText);
            }
        };

        focusChangedCallback = [&](Item &item) {
            setFocusItem(focus ? vBox : nullptr);
            return true;
        };

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (event.state != gui::InputEvent::State::keyReleasedShort) {
                return false;
            }

            return vBox->onInput(event);
        };

        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    }

    auto InformationWidget::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool
    {
        vBox->setPosition(0, 0);
        vBox->setSize(newDim.w, newDim.h);
        vBox->resizeItems();

        return true;
    }
} /* namespace gui */
