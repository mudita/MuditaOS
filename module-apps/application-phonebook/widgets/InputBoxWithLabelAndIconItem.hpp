#pragma once

#include "application-phonebook/data/PhonebookInternals.hpp"
#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/widgets/ContactListItem.hpp"

#include <BoxLayout.hpp>
#include <Image.hpp>
#include <Label.hpp>
#include <ListItem.hpp>

namespace gui
{
    class InputBoxWithLabelAndIconItem : public ContactListItem
    {

        phonebookInternals::ListItemName listItemName;

      public:
        InputBoxWithLabelAndIconItem(phonebookInternals::ListItemName listItemName,
                                     std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                                     std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr);
        ~InputBoxWithLabelAndIconItem() override = default;
        auto onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool override;
        gui::HBox *hBox              = nullptr;
        gui::Label *inputBoxLabel    = nullptr;
        gui::Label *descriptionLabel = nullptr;
        gui::Image *iconImage        = nullptr;
        gui::Image *tickImage        = nullptr;

      private:
        std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr;
        std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr;

        void applyItemNameSpecificSettings();

        void speedDialKeyHandler();
        void addToFavouritesHandler();
        void addToICEHandler();
    };

} /* namespace gui */
