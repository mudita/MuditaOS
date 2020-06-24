#include "InputBoxWithLabelAndIconItem.hpp"

#include "application-phonebook/data/PhonebookStyle.hpp"

#include <module-utils/i18/i18.hpp>

namespace gui
{
    InputBoxWithLabelAndIconItem::InputBoxWithLabelAndIconItem(phonebookInternals::ListItemName listItemName)
        : listItemName(listItemName)
    {
        setMinimumSize(phonebookStyle::inputBoxWithLabelAndIconItem::w,
                       phonebookStyle::inputBoxWithLabelAndIconItem::h);
        setMaximumSize(phonebookStyle::inputBoxWithLabelAndIconItem::w,
                       phonebookStyle::inputBoxWithLabelAndIconItem::h);

        hBox = new gui::HBox(this, 0, 0, phonebookStyle::inputBoxWithLabelAndIconItem::w, 0);
        hBox->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        hBox->setPenFocusWidth(style::window::default_border_focucs_w);
        hBox->setPenWidth(style::window::messages::sms_border_no_focus);

        inputBoxLabel = new gui::Label(hBox, 0, 0, 0, 0);
        inputBoxLabel->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        inputBoxLabel->setAlignment(Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_TOP));
        inputBoxLabel->setFont(style::window::font::medium);
        inputBoxLabel->activeItem = false;

        descriptionLabel = new gui::Label(hBox, 0, 0, 0, 0);
        descriptionLabel->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        descriptionLabel->setAlignment(Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_TOP));
        descriptionLabel->setFont(style::window::font::medium);
        descriptionLabel->activeItem = false;

        iconImage             = new gui::Image(hBox, 0, 0, 0, 0);
        iconImage->activeItem = false;

        tickImage = new gui::Image(hBox, 0, 0, 0, 0);
        tickImage->setVisible(false);
        tickImage->activeItem = false;

        applyItemNameSpecificSettings();

        this->focusChangedCallback = [&](gui::Item &item) {
            if (this->focus) {
                setFocusItem(inputBoxLabel);
            }
            else {
                setFocusItem(nullptr);
            }
            return true;
        };

        this->inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            return inputBoxLabel->onInput(event);
        };

        this->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        hBox->addWidget(inputBoxLabel);
        hBox->addWidget(descriptionLabel);
        hBox->addWidget(iconImage);
    }

    InputBoxWithLabelAndIconItem::InputBoxWithLabelAndIconItem(const InputBoxWithLabelAndIconItem &item)
        : InputBoxWithLabelAndIconItem(item.listItemName)
    {}

    auto InputBoxWithLabelAndIconItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool
    {
        hBox->setPosition(0, 0);
        hBox->setSize(newDim.w, newDim.h);

        inputBoxLabel->setPosition(0, 0);
        inputBoxLabel->setSize(phonebookStyle::inputBoxWithLabelAndIconItem::input_box_w,
                               phonebookStyle::inputBoxWithLabelAndIconItem::input_box_h);

        descriptionLabel->setPosition(phonebookStyle::inputBoxWithLabelAndIconItem::description_label_x, 0);
        descriptionLabel->setSize(phonebookStyle::inputBoxWithLabelAndIconItem::description_label_w,
                                  phonebookStyle::inputBoxWithLabelAndIconItem::description_label_h);

        iconImage->setPosition(phonebookStyle::inputBoxWithLabelAndIconItem::icon_image_x, 0);
        iconImage->setSize(phonebookStyle::inputBoxWithLabelAndIconItem::icon_image_w,
                           phonebookStyle::inputBoxWithLabelAndIconItem::icon_image_h);

        tickImage->setPosition(phonebookStyle::inputBoxWithLabelAndIconItem::tick_image_x, 0);
        tickImage->setSize(phonebookStyle::inputBoxWithLabelAndIconItem::tick_image_w,
                           phonebookStyle::inputBoxWithLabelAndIconItem::tick_image_h);

        return true;
    }

    auto InputBoxWithLabelAndIconItem::clone() -> gui::ListItem *
    {
        return new InputBoxWithLabelAndIconItem(*this);
    }
    void InputBoxWithLabelAndIconItem::applyItemNameSpecificSettings()
    {
        switch (listItemName) {
        case phonebookInternals::ListItemName::SpeedDialKey:
            descriptionLabel->setText(utils::localize.get("app_phonebook_new_speed_dial_key"));
            iconImage->set("small_circle");
            break;
        case phonebookInternals::ListItemName::AddToFavourites:
            descriptionLabel->setText(utils::localize.get("app_phonebook_new_add_to_fav"));
            iconImage->set("small_heart");
            tickImage->set("small_tick");
            tickImage->setVisible(true);
            break;
        case phonebookInternals::ListItemName::AddToICE:
            descriptionLabel->setText(utils::localize.get("app_phonebook_new_add_to_ice"));
            iconImage->set("small_heart");
            tickImage->set("small_tick");
            tickImage->setVisible(true);
            break;
        default:
            LOG_ERROR("Incorrect List Item Name!");
            break;
        }
    }

} /* namespace gui */
