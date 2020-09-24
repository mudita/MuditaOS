#pragma once
#include "CalendarListItem.hpp"
#include <Label.hpp>
#include <Text.hpp>
#include <BoxLayout.hpp>

namespace gui
{
    namespace timeConstants
    {
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
        gui::EventTimeItem *secondItem = nullptr;

        std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr;
        std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr;

        void applyInputCallbacks();
        void prepareForTimeMode();
        bool isPm(const std::string &text);
        void validateHour();
        void validateHourFor12hMode(std::chrono::hours start_hour,
                                    std::chrono::minutes end_hour,
                                    uint32_t start_minutes,
                                    uint32_t end_minutes);
        void validateHourFor24hMode(std::chrono::hours start_hour,
                                    std::chrono::minutes end_hour,
                                    uint32_t start_minutes,
                                    uint32_t end_minutes);

      public:
        EventTimeItem(const std::string &description,
                      bool mode24H,
                      std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                      std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr);
        virtual ~EventTimeItem() override = default;

        void setConnectionToSecondItem(gui::EventTimeItem *item);
        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
