#pragma once

#include "application-phonebook/data/PhonebookInternals.hpp"
#include "application-phonebook/data/PhonebookItemData.hpp"
#include "application-phonebook/widgets/ContactListItem.hpp"

#include <ListItem.hpp>
#include <Text.hpp>
#include <TextFixedSize.hpp>

namespace gui
{
    class InputLinesWithLabelIWidget : public ContactListItem
    {
        phonebookInternals::ListItemName listItemName;

      public:
        InputLinesWithLabelIWidget(phonebookInternals::ListItemName listItemName,
                                   std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                                   std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr,
                                   std::function<void()> selectSpecialCharacter                 = nullptr,
                                   std::function<void()> contentChanged                         = nullptr,
                                   unsigned int lines                                           = 1);

        ~InputLinesWithLabelIWidget() override = default;
        auto onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool override;
        VBox *vBox               = nullptr;
        Label *titleLabel        = nullptr;
        TextFixedSize *inputText = nullptr;

      private:
        std::function<void()> checkTextContent = nullptr;
        void applyItemNameSpecificSettings();

        void firstNameHandler();
        void secondNameHandler();
        void numberHandler();
        void secondNumberHandler();
        void emailHandler();
        void addressHandler();
        void noteHandler();
    };

} /* namespace gui */
