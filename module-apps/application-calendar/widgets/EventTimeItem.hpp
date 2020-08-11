#pragma once
#include "CalendarListItem.hpp"
#include <Label.hpp>
#include <Text.hpp>
#include <BoxLayout.hpp>

namespace gui
{
    namespace timeConstants
    {
        const inline int max_hour_24H_mode   = 23;
        const inline int max_hour_12H_mode   = 12;
        const inline int max_minutes         = 59;
        const inline std::string before_noon = "AM";
        const inline std::string after_noon  = "PM";
    } // namespace timeConstants
    class EventTimeItem : public CalendarListItem
    {
        gui::VBox *vBox              = nullptr;
        gui::HBox *hBox              = nullptr;
        gui::Label *colonLabel       = nullptr;
        gui::Label *descriptionLabel = nullptr;
        gui::Text *hourInput         = nullptr;
        gui::Text *minuteInput       = nullptr;
        gui::Label *mode12hInput     = nullptr;
        bool mode24H                 = false;

        std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr;
        std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr;

        void applyInputCallbacks();
        void prepareForTimeMode();

      public:
        EventTimeItem(const std::string &description,
                      bool mode24H,
                      std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                      std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr);
        virtual ~EventTimeItem() override = default;

        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
