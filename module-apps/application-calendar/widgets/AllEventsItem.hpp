#pragma once
#include <ListItem.hpp>
#include <Label.hpp>
#include <BoxLayout.hpp>
#include <module-db/Interface/EventsRecord.hpp>

namespace gui
{
    class AllEventsItem : public ListItem
    {
        gui::HBox *hBox         = nullptr;
        gui::Label *startTime   = nullptr;
        gui::Label *description = nullptr;
        std::shared_ptr<EventsRecord> record;

      public:
        AllEventsItem();
        virtual ~AllEventsItem() override = default;

        void setMarkerItem(UTF8 text);
        [[nodiscard]] UTF8 getLabelMarker() const;
        void setEvent(std::shared_ptr<EventsRecord> record);

        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
