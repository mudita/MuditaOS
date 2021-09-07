// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "InputBoxWithLabelAndIconWidget.hpp"

#include "application-phonebook/data/PhonebookStyle.hpp"

#include <BottomBar.hpp>
#include <i18n/i18n.hpp>

#include <utility>

namespace gui
{
    InputBoxWithLabelAndIconWidget::InputBoxWithLabelAndIconWidget(
        phonebookInternals::ListItemName listItemName,
        std::function<void(const UTF8 &)> bottomBarTemporaryMode,
        std::function<void()> bottomBarRestoreFromTemporaryMode)
        : listItemName(listItemName), bottomBarTemporaryMode(std::move(bottomBarTemporaryMode)),
          bottomBarRestoreFromTemporaryMode(std::move(bottomBarRestoreFromTemporaryMode))
    {
        setMinimumSize(phonebookStyle::inputBoxWithLabelAndIconIWidget::w,
                       phonebookStyle::inputBoxWithLabelAndIconIWidget::h);

        setMargins(gui::Margins(style::widgets::leftMargin, style::margins::big, 0, 0));

        hBox = new gui::HBox(this, 0, 0, 0, 0);
        hBox->setEdges(gui::RectangleEdge::None);
        hBox->setPenFocusWidth(style::window::default_border_focus_w);
        hBox->setPenWidth(style::window::default_border_rect_no_focus);

        inputBoxLabel = new gui::Label(hBox, 0, 0, 0, 0);
        inputBoxLabel->setMinimumSize(phonebookStyle::inputBoxWithLabelAndIconIWidget::input_box_w,
                                      phonebookStyle::inputBoxWithLabelAndIconIWidget::input_box_h);
        inputBoxLabel->setMargins(
            gui::Margins(0, 0, phonebookStyle::inputBoxWithLabelAndIconIWidget::input_box_right_margin, 0));
        inputBoxLabel->setEdges(gui::RectangleEdge::Bottom);
        inputBoxLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        inputBoxLabel->setFont(style::window::font::medium);
        inputBoxLabel->activeItem = false;

        tickImage = new gui::Image(hBox, 0, 0, 0, 0);
        tickImage->setAlignment(Alignment(gui::Alignment::Vertical::Center));
        // Not ideal -> best solution would be to create separate widget with image inside box.
        tickImage->setMargins(gui::Margins(phonebookStyle::inputBoxWithLabelAndIconIWidget::tick_image_left_margin,
                                           0,
                                           phonebookStyle::inputBoxWithLabelAndIconIWidget::tick_image_right_margin,
                                           0));
        tickImage->setVisible(false);
        tickImage->activeItem = false;

        descriptionLabel = new gui::Label(hBox, 0, 0, 0, 0);
        descriptionLabel->setMaximumSize(phonebookStyle::inputBoxWithLabelAndIconIWidget::description_label_w,
                                         phonebookStyle::inputBoxWithLabelAndIconIWidget::description_label_h);
        descriptionLabel->setMargins(
            gui::Margins(0, 0, phonebookStyle::inputBoxWithLabelAndIconIWidget::description_label_right_margin, 0));
        descriptionLabel->setEdges(gui::RectangleEdge::None);
        descriptionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        descriptionLabel->setFont(style::window::font::medium);
        descriptionLabel->activeItem = false;

        iconImage = new gui::Image(hBox, 0, 0, 0, 0);
        iconImage->setAlignment(Alignment(gui::Alignment::Vertical::Center));
        iconImage->activeItem = false;

        applyItemNameSpecificSettings();

        setEdges(gui::RectangleEdge::None);

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            hBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }

    void InputBoxWithLabelAndIconWidget::applyItemNameSpecificSettings()
    {
        switch (listItemName) {
        case phonebookInternals::ListItemName::SpeedDialKey:
            speedDialKeyHandler();
            break;

        case phonebookInternals::ListItemName::AddToFavourites:
            addToFavouritesHandler();
            break;

        case phonebookInternals::ListItemName::AddToICE:
            addToICEHandler();
            break;

        default:
            LOG_ERROR("Incorrect List Item Name!");
            break;
        }
    }

    void InputBoxWithLabelAndIconWidget::speedDialKeyHandler()
    {
        descriptionLabel->setText(utils::translate("app_phonebook_new_speed_dial_key"));
        iconImage->set("speed_dial_empty_W_M");

        focusChangedCallback = [&](gui::Item &item) {
            if (focus) {
                setFocusItem(inputBoxLabel);
                descriptionLabel->setFont(style::window::font::mediumbold);
            }
            else {
                setFocusItem(nullptr);
                descriptionLabel->setFont(style::window::font::medium);
            }
            return true;
        };

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (!event.isShortRelease()) {
                return false;
            }
            if (event.isDigit()) {
                inputBoxLabel->setText(std::to_string(event.numericValue()));
                return true;
            }
            if (event.is(KeyCode::KEY_PND)) {
                inputBoxLabel->clear();
                return true;
            }
            return false;
        };

        onSaveCallback = [&](std::shared_ptr<ContactRecord> contact) { contact->speeddial = inputBoxLabel->getText(); };
        onLoadCallback = [&](std::shared_ptr<ContactRecord> contact) { inputBoxLabel->setText(contact->speeddial); };
    }

    void InputBoxWithLabelAndIconWidget::addToFavouritesHandler()
    {
        descriptionLabel->setText(utils::translate("app_phonebook_new_add_to_fav"));
        iconImage->set("small_heart_W_M");
        tickImage->set("small_tick_W_M");

        focusChangedCallback = [&](gui::Item &item) {
            if (focus) {
                setFocusItem(inputBoxLabel);
                descriptionLabel->setFont(style::window::font::mediumbold);
                if (tickImage->visible) {
                    bottomBarTemporaryMode(utils::translate("app_phonebook_uncheck"));
                }
                else {
                    bottomBarTemporaryMode(utils::translate("app_phonebook_check"));
                }
            }
            else {
                setFocusItem(nullptr);
                descriptionLabel->setFont(style::window::font::medium);
                bottomBarRestoreFromTemporaryMode();
            }
            return true;
        };

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (event.isShortRelease(gui::KeyCode::KEY_LF)) {
                tickImage->setVisible(!tickImage->visible);
                if (tickImage->visible) {
                    bottomBarTemporaryMode(utils::translate("app_phonebook_uncheck"));
                }
                else {
                    bottomBarTemporaryMode(utils::translate("app_phonebook_check"));
                }
                hBox->resizeItems();
                return true;
            }
            return false;
        };

        onSaveCallback = [&](std::shared_ptr<ContactRecord> contact) { contact->addToFavourites(tickImage->visible); };
        onLoadCallback = [&](std::shared_ptr<ContactRecord> contact) {
            tickImage->visible = contact->isOnFavourites();
        };
    }
    void InputBoxWithLabelAndIconWidget::addToICEHandler()
    {
        descriptionLabel->setText(utils::translate("app_phonebook_new_add_to_ice"));
        iconImage->set("ice");
        tickImage->set("small_tick_W_M");

        focusChangedCallback = [&](gui::Item &item) {
            if (focus) {
                setFocusItem(inputBoxLabel);
                descriptionLabel->setFont(style::window::font::mediumbold);
                if (tickImage->visible) {
                    bottomBarTemporaryMode(utils::translate("app_phonebook_uncheck"));
                }
                else {
                    bottomBarTemporaryMode(utils::translate("app_phonebook_check"));
                }
            }
            else {
                setFocusItem(nullptr);
                descriptionLabel->setFont(style::window::font::medium);
                bottomBarRestoreFromTemporaryMode();
            }
            return true;
        };

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (event.isShortRelease(gui::KeyCode::KEY_LF)) {
                tickImage->setVisible(!tickImage->visible);
                if (tickImage->visible) {
                    bottomBarTemporaryMode(utils::translate("app_phonebook_uncheck"));
                }
                else {
                    bottomBarTemporaryMode(utils::translate("app_phonebook_check"));
                }
                hBox->resizeItems();
                return true;
            }
            return false;
        };
        onSaveCallback = [&](std::shared_ptr<ContactRecord> contact) { contact->addToIce(tickImage->visible); };
        onLoadCallback = [&](std::shared_ptr<ContactRecord> contact) { tickImage->visible = contact->isOnIce(); };
    }

} /* namespace gui */
