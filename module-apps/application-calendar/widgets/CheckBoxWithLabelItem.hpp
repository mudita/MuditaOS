// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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

        virtual void applyCallbacks();
        void setCheckBoxes();

      public:
        CheckBoxWithLabelItem(app::Application *application,
                              const std::string &description,
                              std::shared_ptr<WeekDaysRepeatData> data = nullptr);
        virtual ~CheckBoxWithLabelItem() override = default;

        gui::Label *descriptionLabel = nullptr;
        gui::CheckBox *checkBox      = nullptr;
    };

} /* namespace gui */
