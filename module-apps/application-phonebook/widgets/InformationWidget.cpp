// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "InformationWidget.hpp"

#include "AppWindow.hpp"
#include "application-phonebook/data/PhonebookStyle.hpp"
#include "widgets/TextWithIconsWidget.hpp"
#include "widgets/ActiveIconFactory.hpp"

#include <ContactRecord.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    InformationWidget::InformationWidget(app::ApplicationCommon *app)
    {
        setMargins(gui::Margins(style::widgets::leftMargin, style::margins::huge, 0, 0));

        setMinimumSize(phonebookStyle::informationWidget::w,
                       phonebookStyle::informationWidget::title_label_h + style::margins::huge);

        vBox = new VBox(this, 0, 0, 0, 0);
        vBox->setEdges(RectangleEdge::None);

        titleLabel = new Label(vBox, 0, 0, 0, 0, utils::translate("app_phonebook_contact_information"));
        titleLabel->setMinimumSize(phonebookStyle::informationWidget::w,
                                   phonebookStyle::informationWidget::title_label_h);
        titleLabel->setEdges(RectangleEdge::None);
        titleLabel->setMargins(Margins(0, 0, 0, style::margins::very_big));
        titleLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        titleLabel->setFont(style::window::font::verysmall);
        titleLabel->setLineMode(true);
        titleLabel->activeItem = false;

        onLoadCallback = [=](std::shared_ptr<ContactRecord> contact) {
            auto createBox = [=](const utils::PhoneNumber::View &number, const UTF8 &font) {
                auto numberHBox = new TextWithIconsWidget(vBox);
                ActiveIconFactory factory(app);
                numberHBox->addText(number.getFormatted(), font);
                numberHBox->addIcon(factory.makeCallIcon(number));
                numberHBox->addIcon(factory.makeSMSIcon(number));
                return numberHBox;
            };

            if (contact->numbers.size() > 0) {
                setMinimumHeight(widgetMinimumArea.h + phonebookStyle::numbersWithIconsWidget::h);
                primaryNumberHBox = createBox(contact->numbers[0].number, style::window::font::mediumbold);
            }
            if (contact->numbers.size() > 1) {
                setMinimumHeight(widgetMinimumArea.h + phonebookStyle::numbersWithIconsWidget::h);
                secondaryNumberHBox = createBox(contact->numbers[1].number, style::window::font::medium);

                // Set proper navigation if second number is present
                primaryNumberHBox->icons[0]->setNavigationItem(NavigationDirection::DOWN,
                                                               secondaryNumberHBox->icons[0]);
                primaryNumberHBox->icons[1]->setNavigationItem(NavigationDirection::DOWN,
                                                               secondaryNumberHBox->icons[1]);

                secondaryNumberHBox->icons[0]->setNavigationItem(NavigationDirection::UP, primaryNumberHBox->icons[0]);
                secondaryNumberHBox->icons[1]->setNavigationItem(NavigationDirection::UP, primaryNumberHBox->icons[1]);
            }
            if (contact->mail.length() > 0) {
                setMinimumHeight(widgetMinimumArea.h + phonebookStyle::informationWidget::email_text_h +
                                 style::margins::very_big);

                emailText = new Text(vBox, 0, 0, 0, 0);
                emailText->setMaximumSize(phonebookStyle::informationWidget::w,
                                          phonebookStyle::informationWidget::email_text_h);
                emailText->setMargins(Margins(0, style::margins::very_big, 0, 0));
                emailText->setFont(style::window::font::medium);
                emailText->setEditMode(EditMode::Browse);
                emailText->setEdges(RectangleEdge::None);
                emailText->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
                emailText->setText(contact->mail);
                emailText->activeItem = false;
            }
        };

        focusChangedCallback = [&, app](Item &item) {
            if (focus) {
                if (savedFocusItem == nullptr) {
                    setFocusItem(vBox);
                }
                else {
                    savedFocusItem->parent->focusChangedCallback(*savedFocusItem);
                }
            }
            else {
                savedFocusItem = getFocusItem();
                app->getCurrentWindow()->navBarRestoreFromTemporaryMode();
            }
            return true;
        };

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (!event.isShortRelease()) {
                return false;
            }

            // Clear VBox down navigation if second number is present.
            if (secondaryNumberHBox != nullptr) {
                primaryNumberHBox->clearNavigationItem(NavigationDirection::DOWN);
            }

            return vBox->onInput(event);
        };

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            vBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };

        setEdges(RectangleEdge::None);
    }
} /* namespace gui */
