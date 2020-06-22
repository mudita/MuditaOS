#pragma once

#include "application-phonebook/data/PhonebookInternals.hpp"

#include <ListItem.hpp>
#include <module-gui/gui/widgets/Text.hpp>

namespace gui
{
    class InputLineWithLabelItem : public ListItem
    {

        phonebookInternals::ListItemName listItemName;

      public:
        InputLineWithLabelItem(phonebookInternals::ListItemName listItemName);
        InputLineWithLabelItem(const InputLineWithLabelItem &item);

        ~InputLineWithLabelItem() override = default;
        auto onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool override;
        gui::VBox *vBox   = nullptr;
        gui::Label *label = nullptr;
        gui::Text *text   = nullptr;
        auto clone() -> gui::ListItem * override;
    };

} /* namespace gui */
