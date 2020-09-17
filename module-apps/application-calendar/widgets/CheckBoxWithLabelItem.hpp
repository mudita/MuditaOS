#pragma once
#include "CalendarListItem.hpp"
#include "Application.hpp"
#include "module-apps/application-calendar/data/CalendarData.hpp"
#include <Label.hpp>
#include <Image.hpp>
#include <BoxLayout.hpp>
#include <BottomBar.hpp>
#include <CheckBox.hpp>

namespace gui
{
    class CheckBoxWithLabelItem : public CalendarListItem
    {
        gui::HBox *hBox           = nullptr;
        app::Application *app     = nullptr;
        std::shared_ptr<WeekDaysRepeatData> checkBoxData = nullptr;
        bool checkIsOnLeftBarSide = false;

        virtual void applyCallbacks();
        void setCheckBoxes();

      public:
        CheckBoxWithLabelItem(app::Application *application,
                              const std::string &description,
                              std::shared_ptr<WeekDaysRepeatData> data = nullptr,
                              bool checkIsOnLeftBarSide                = false);
        virtual ~CheckBoxWithLabelItem() override = default;

        gui::Label *descriptionLabel = nullptr;
        gui::CheckBox *checkBox      = nullptr;

        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
