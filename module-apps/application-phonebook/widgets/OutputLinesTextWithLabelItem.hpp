#pragma once

#include "application-phonebook/data/PhonebookInternals.hpp"
#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/widgets/ContactListItem.hpp"

#include <ListItem.hpp>
#include <Text.hpp>

namespace gui
{
    class OutputLinesTextWithLabelItem : public ContactListItem
    {
        phonebookInternals::ListItemName listItemName;

      public:
        OutputLinesTextWithLabelItem(phonebookInternals::ListItemName listItemName);

        ~OutputLinesTextWithLabelItem() override = default;
        auto onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool override;
        VBox *vBox          = nullptr;
        Label *titleLabel   = nullptr;
        Text *multilineText = nullptr;

        auto handleRequestResize(const Item *, unsigned short request_w, unsigned short request_h) -> Size override;

      private:
        void applyItemNameSpecificSettings();

        void addressHandler();
        void noteHandler();
    };

} /* namespace gui */
