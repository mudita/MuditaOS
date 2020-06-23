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
        InputLineWithLabelItem(phonebookInternals::ListItemName listItemName,
                               std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                               std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr,
                               std::function<void()> selectSpecialCharacter                 = nullptr);
        InputLineWithLabelItem(const InputLineWithLabelItem &item);

        ~InputLineWithLabelItem() override = default;
        auto onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool override;
        VBox *vBox        = nullptr;
        Label *titleLabel = nullptr;
        Text *inputText   = nullptr;
        auto clone() -> ListItem *;

      private:
        void applyItemNameSpecificSettings();
    };

} /* namespace gui */
