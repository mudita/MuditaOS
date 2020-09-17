#pragma once
#include <Label.hpp>
#include <ListItem.hpp>
#include <module-db/Interface/EventsRecord.hpp>
#include <BoxLayout.hpp>
#include <module-gui/gui/widgets/Image.hpp>
#include <Image.hpp>

namespace gui

{
    class DayEventsItem : public ListItem
    {
        gui::VBox *vBox         = nullptr;
        gui::HBox *hBox         = nullptr;
        gui::Label *title       = nullptr;
        gui::Label *description = nullptr;
        gui::Image *clock       = nullptr;

      public:
        std::shared_ptr<EventsRecord> record;
        DayEventsItem();
        virtual ~DayEventsItem() override = default;

        // virtual methods from Item
        void setEvent(std::shared_ptr<EventsRecord> record);
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
