#include "InputBoxWithLabelAndIconWidget.hpp"

#include "application-phonebook/data/PhonebookStyle.hpp"

#include <BottomBar.hpp>
#include <module-utils/i18/i18.hpp>

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

        setMargins(gui::Margins(0, style::margins::big, 0, 0));

        hBox = new gui::HBox(this, 0, 0, phonebookStyle::inputBoxWithLabelAndIconIWidget::w, 0);
        hBox->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        hBox->setPenFocusWidth(style::window::default_border_focus_w);
        hBox->setPenWidth(style::window::messages::sms_border_no_focus);

        inputBoxLabel = new gui::Label(hBox, 0, 0, 0, 0);
        inputBoxLabel->setMinimumSize(phonebookStyle::inputBoxWithLabelAndIconIWidget::input_box_w,
                                      phonebookStyle::inputBoxWithLabelAndIconIWidget::input_box_h);
        inputBoxLabel->setMargins(
            gui::Margins(0, 0, phonebookStyle::inputBoxWithLabelAndIconIWidget::input_box_right_margin, 0));
        inputBoxLabel->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
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
        descriptionLabel->setMinimumSize(phonebookStyle::inputBoxWithLabelAndIconIWidget::description_label_w,
                                         phonebookStyle::inputBoxWithLabelAndIconIWidget::description_label_h);
        descriptionLabel->setMargins(
            gui::Margins(0, 0, phonebookStyle::inputBoxWithLabelAndIconIWidget::description_label_right_margin, 0));
        descriptionLabel->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        descriptionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        descriptionLabel->setFont(style::window::font::medium);
        descriptionLabel->activeItem = false;

        iconImage = new gui::Image(hBox, 0, 0, 0, 0);
        iconImage->setAlignment(Alignment(gui::Alignment::Vertical::Center));
        iconImage->activeItem = false;

        applyItemNameSpecificSettings();

        setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    }

    auto InputBoxWithLabelAndIconWidget::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
        -> bool
    {
        hBox->setPosition(0, 0);
        hBox->setSize(newDim.w, newDim.h);

        return true;
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
        descriptionLabel->setText(utils::localize.get("app_phonebook_new_speed_dial_key"));
        iconImage->set("speed_dial_empty_W_M");

        focusChangedCallback = [&](gui::Item &item) {
            if (focus) {
                setFocusItem(inputBoxLabel);
            }
            else {
                setFocusItem(nullptr);
            }
            return true;
        };

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (event.state != gui::InputEvent::State::keyReleasedShort) {
                return false;
            }
            if (toNumeric(event.keyCode) != -1) {
                inputBoxLabel->setText(std::to_string(toNumeric(event.keyCode)));
                return true;
            }
            if (event.keyCode == KeyCode::KEY_PND) {
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
        descriptionLabel->setText(utils::localize.get("app_phonebook_new_add_to_fav"));
        iconImage->set("small_heart_W_M");
        tickImage->set("small_tick_W_M");

        focusChangedCallback = [&](gui::Item &item) {
            if (focus) {
                setFocusItem(inputBoxLabel);
                if (tickImage->visible) {
                    bottomBarTemporaryMode(utils::localize.get("app_phonebook_uncheck"));
                }
                else {
                    bottomBarTemporaryMode(utils::localize.get("app_phonebook_check"));
                }
            }
            else {
                setFocusItem(nullptr);
                bottomBarRestoreFromTemporaryMode();
            }
            return true;
        };

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (event.state != gui::InputEvent::State::keyReleasedShort) {
                return false;
            }
            if (event.keyCode == gui::KeyCode::KEY_LF) {
                tickImage->setVisible(!tickImage->visible);
                if (tickImage->visible) {
                    bottomBarTemporaryMode(utils::localize.get("app_phonebook_uncheck"));
                }
                else {
                    bottomBarTemporaryMode(utils::localize.get("app_phonebook_check"));
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
        descriptionLabel->setText(utils::localize.get("app_phonebook_new_add_to_ice"));
        iconImage->set("ice");
        tickImage->set("small_tick_W_M");

        focusChangedCallback = [&](gui::Item &item) {
            if (focus) {
                setFocusItem(inputBoxLabel);
                if (tickImage->visible) {
                    bottomBarTemporaryMode(utils::localize.get("app_phonebook_uncheck"));
                }
                else {
                    bottomBarTemporaryMode(utils::localize.get("app_phonebook_check"));
                }
            }
            else {
                setFocusItem(nullptr);
                bottomBarRestoreFromTemporaryMode();
            }
            return true;
        };

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (event.state != gui::InputEvent::State::keyReleasedShort) {
                return false;
            }

            if (event.keyCode == gui::KeyCode::KEY_LF) {
                tickImage->setVisible(!tickImage->visible);
                if (tickImage->visible) {
                    bottomBarTemporaryMode(utils::localize.get("app_phonebook_uncheck"));
                }
                else {
                    bottomBarTemporaryMode(utils::localize.get("app_phonebook_check"));
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
