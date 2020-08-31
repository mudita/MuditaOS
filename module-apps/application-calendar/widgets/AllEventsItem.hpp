#pragma once
#include <ListItem.hpp>
#include <Label.hpp>
#include <BoxLayout.hpp>

namespace gui
{
    class AllEventsItem : public ListItem
    {
        gui::HBox *hBox         = nullptr;
        gui::Label *startTime   = nullptr;
        gui::Label *description = nullptr;

      public:
        AllEventsItem();
        virtual ~AllEventsItem() override = default;

        void setMarkerItem(UTF8 text);
        [[nodiscard]] UTF8 getLabelMarker() const;

        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
