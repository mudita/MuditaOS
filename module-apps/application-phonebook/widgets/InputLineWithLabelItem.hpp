#pragma once

#include "application-phonebook/data/PhonebookInternals.hpp"
#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/widgets/ContactListItem.hpp"

#include <ListItem.hpp>
#include <Text.hpp>

namespace gui
{
    class InputLineWithLabelItem : public ContactListItem
    {
        phonebookInternals::ListItemName listItemName;

      public:
        InputLineWithLabelItem(phonebookInternals::ListItemName listItemName,
                               std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                               std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr,
                               std::function<void()> selectSpecialCharacter                 = nullptr);

        ~InputLineWithLabelItem() override = default;
        auto onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool override;
        VBox *vBox        = nullptr;
        Label *titleLabel = nullptr;
        Text *inputText   = nullptr;

      private:
        void applyItemNameSpecificSettings();

        void firstNameHandler();
        void secondNameHandler();
        void numberHandler();
        void otherNumberHandler();
        void emailHandler();
        void addressHandler();
        void noteHandler();
    };

} /* namespace gui */
