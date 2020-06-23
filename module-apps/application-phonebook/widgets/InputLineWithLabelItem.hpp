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
        VBox *vBox        = nullptr;
        Label *titleLabel = nullptr;
        Text *inputText   = nullptr;
        auto clone() -> ListItem *;
    };

} /* namespace gui */
