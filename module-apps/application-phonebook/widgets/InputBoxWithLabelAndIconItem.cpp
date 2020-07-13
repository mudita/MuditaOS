#include "InputBoxWithLabelAndIconItem.hpp"

#include "application-phonebook/data/PhonebookStyle.hpp"

#include <BottomBar.hpp>
#include <module-utils/i18/i18.hpp>

#include <utility>

namespace gui
{
    InputBoxWithLabelAndIconItem::InputBoxWithLabelAndIconItem(phonebookInternals::ListItemName listItemName,
                                                               std::function<void(const UTF8 &)> bottomBarTemporaryMode,
                                                               std::function<void()> bottomBarRestoreFromTemporaryMode)
        : listItemName(listItemName), bottomBarTemporaryMode(std::move(bottomBarTemporaryMode)),
          bottomBarRestoreFromTemporaryMode(std::move(bottomBarRestoreFromTemporaryMode))
    {
        setMinimumSize(phonebookStyle::inputBoxWithLabelAndIconItem::w,
                       phonebookStyle::inputBoxWithLabelAndIconItem::h);
        setMaximumSize(phonebookStyle::inputBoxWithLabelAndIconItem::w,
                       phonebookStyle::inputBoxWithLabelAndIconItem::h);
        setMargins(gui::Margins(0, style::margins::big, 0, 0));

        hBox = new gui::HBox(this, 0, 0, phonebookStyle::inputBoxWithLabelAndIconItem::w, 0);
        hBox->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        hBox->setPenFocusWidth(style::window::default_border_focus_w);
        hBox->setPenWidth(style::window::messages::sms_border_no_focus);

        inputBoxLabel = new gui::Label(hBox, 0, 0, 0, 0);
        inputBoxLabel->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        inputBoxLabel->setAlignment(Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));
        inputBoxLabel->setFont(style::window::font::medium);
        inputBoxLabel->activeItem = false;

        descriptionLabel = new gui::Label(hBox, 0, 0, 0, 0);
        descriptionLabel->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        descriptionLabel->setAlignment(Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_CENTER));
        descriptionLabel->setFont(style::window::font::medium);
        descriptionLabel->activeItem = false;

        iconImage             = new gui::Image(hBox, 0, 0, 0, 0);
        iconImage->activeItem = false;

        tickImage = new gui::Image(hBox, 0, 0, 0, 0);
        tickImage->setVisible(false);
        tickImage->activeItem = false;

        applyItemNameSpecificSettings();

        setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    }

    auto InputBoxWithLabelAndIconItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool
    {
        hBox->setPosition(0, 0);
        hBox->setSize(newDim.w, newDim.h);

        inputBoxLabel->setArea(BoundingBox(0,
                                           newDim.h - phonebookStyle::inputBoxWithLabelAndIconItem::input_box_h,
                                           phonebookStyle::inputBoxWithLabelAndIconItem::input_box_w,
                                           phonebookStyle::inputBoxWithLabelAndIconItem::input_box_h));

        descriptionLabel->setArea(
            BoundingBox(phonebookStyle::inputBoxWithLabelAndIconItem::description_label_x,
                        (newDim.h - phonebookStyle::inputBoxWithLabelAndIconItem::description_label_h) / 2,
                        phonebookStyle::inputBoxWithLabelAndIconItem::description_label_w,
                        phonebookStyle::inputBoxWithLabelAndIconItem::description_label_h));

        iconImage->setArea(BoundingBox(phonebookStyle::inputBoxWithLabelAndIconItem::icon_image_x,
                                       (newDim.h - phonebookStyle::inputBoxWithLabelAndIconItem::icon_image_h) / 2,
                                       phonebookStyle::inputBoxWithLabelAndIconItem::icon_image_w,
                                       phonebookStyle::inputBoxWithLabelAndIconItem::icon_image_h));

        tickImage->setArea(BoundingBox(phonebookStyle::inputBoxWithLabelAndIconItem::tick_image_x,
                                       (newDim.h - phonebookStyle::inputBoxWithLabelAndIconItem::tick_image_h) / 2,
                                       phonebookStyle::inputBoxWithLabelAndIconItem::tick_image_w,
                                       phonebookStyle::inputBoxWithLabelAndIconItem::tick_image_h));

        return true;
    }

    void InputBoxWithLabelAndIconItem::applyItemNameSpecificSettings()
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

    void InputBoxWithLabelAndIconItem::speedDialKeyHandler()
    {
        descriptionLabel->setText(utils::localize.get("app_phonebook_new_speed_dial_key"));
        iconImage->set("small_circle");

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

    void InputBoxWithLabelAndIconItem::addToFavouritesHandler()
    {
        descriptionLabel->setText(utils::localize.get("app_phonebook_new_add_to_fav"));
        iconImage->set("small_heart");
        tickImage->set("small_tick");

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
            }
            return false;
        };

        onSaveCallback = [&](std::shared_ptr<ContactRecord> contact) { contact->isOnFavourites = tickImage->visible; };
        onLoadCallback = [&](std::shared_ptr<ContactRecord> contact) { tickImage->visible = contact->isOnFavourites; };
    }
    void InputBoxWithLabelAndIconItem::addToICEHandler()
    {
        descriptionLabel->setText(utils::localize.get("app_phonebook_new_add_to_ice"));
        iconImage->set("small_heart");
        tickImage->set("small_tick");

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
            }
            return false;
        };
    }

} /* namespace gui */
