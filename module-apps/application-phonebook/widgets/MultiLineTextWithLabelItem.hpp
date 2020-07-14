#pragma once

#include "application-phonebook/data/PhonebookInternals.hpp"
#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/widgets/ContactListItem.hpp"

#include <ListItem.hpp>
#include <Text.hpp>

namespace gui
{
    class MultiLineTextWithLabelItem : public ContactListItem
    {
        phonebookInternals::ListItemName listItemName;

      public:
        MultiLineTextWithLabelItem(phonebookInternals::ListItemName listItemName);

        ~MultiLineTextWithLabelItem() override = default;
        auto onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool override;
        VBox *vBox          = nullptr;
        Label *titleLabel   = nullptr;
        Text *multilineText = nullptr;

      private:
        void applyItemNameSpecificSettings();

        void addressHandler();
        void noteHandler();
    };

} /* namespace gui */
