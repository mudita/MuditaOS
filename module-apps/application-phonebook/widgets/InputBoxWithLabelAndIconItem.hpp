#pragma once

#include "application-phonebook/data/PhonebookInternals.hpp"

#include <BoxLayout.hpp>
#include <Image.hpp>
#include <Label.hpp>
#include <ListItem.hpp>

namespace gui
{
    class InputBoxWithLabelAndIconItem : public ListItem
    {

        phonebookInternals::ListItemName listItemName;

      public:
        InputBoxWithLabelAndIconItem(phonebookInternals::ListItemName listItemName);
        ~InputBoxWithLabelAndIconItem() override = default;
        auto onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool override;
        gui::HBox *hBox              = nullptr;
        gui::Label *inputBoxLabel    = nullptr;
        gui::Label *descriptionLabel = nullptr;
        gui::Image *iconImage        = nullptr;
        gui::Image *tickImage        = nullptr;

      private:
        void applyItemNameSpecificSettings();
    };

} /* namespace gui */
