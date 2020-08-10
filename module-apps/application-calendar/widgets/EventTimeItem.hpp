#pragma once
#include "CalendarListItem.hpp"
#include <Label.hpp>
#include <Text.hpp>
#include <BoxLayout.hpp>

namespace gui
{
    class EventTimeItem : public CalendarListItem
    {
        gui::VBox *vBox              = nullptr;
        gui::HBox *hBox              = nullptr;
        gui::Label *colonLabel       = nullptr;
        gui::Label *descriptionLabel = nullptr;
        gui::Text *hourInput         = nullptr;
        gui::Text *minuteInput       = nullptr;
        gui::Text *mode12hInput      = nullptr;
        bool mode24H                 = false;

        std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr;
        std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr;

      public:
        EventTimeItem(const std::string &description,
                      bool mode24H,
                      std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                      std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr);
        virtual ~EventTimeItem() override = default;

        void prepareForTimeMode();
        void setNavigation();
        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
