#pragma once
#include "CalendarListItem.hpp"
#include <Label.hpp>
#include <Text.hpp>
#include <BoxLayout.hpp>

namespace gui
{
    class TextWithLabelItem : public CalendarListItem
    {
        gui::VBox *vBox              = nullptr;
        gui::Label *descriptionLabel = nullptr;
        gui::Text *textInput         = nullptr;

      public:
        TextWithLabelItem(const std::string &description);
        virtual ~TextWithLabelItem() override = default;

        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
