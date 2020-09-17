#pragma once
#include "Application.hpp"
#include "CalendarListItem.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include <module-apps/application-calendar/data/CalendarData.hpp>
#include <Label.hpp>
#include <Image.hpp>
#include <BoxLayout.hpp>

namespace gui
{
    const std::array<Reminder, 10> reminderTimeOptions = {Reminder::never,
                                                          Reminder::event_time,
                                                          Reminder::five_min_before,
                                                          Reminder::fifteen_min_before,
                                                          Reminder::thirty_min_before,
                                                          Reminder::one_hour_before,
                                                          Reminder::two_hour_before,
                                                          Reminder::one_day_before,
                                                          Reminder::two_days_before,
                                                          Reminder::one_week_before};

    class SeveralOptionsItem : public CalendarListItem
    {
        app::Application *application = nullptr;
        gui::VBox *vBox               = nullptr;
        gui::HBox *hBox               = nullptr;
        gui::Label *optionLabel       = nullptr;
        gui::Label *descriptionLabel  = nullptr;
        gui::Image *leftArrow         = nullptr;
        gui::Image *rightArrow        = nullptr;
        std::vector<std::string> optionsNames;

        unsigned int actualVectorIndex = 0;

        std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr;
        std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr;

      public:
        SeveralOptionsItem(app::Application *app,
                           const std::string &description,
                           std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                           std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr);
        virtual ~SeveralOptionsItem() override = default;

        void prepareOptionsNames();
        void applyCallbacks();
        uint32_t repeatOptionValue = 0;

        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
