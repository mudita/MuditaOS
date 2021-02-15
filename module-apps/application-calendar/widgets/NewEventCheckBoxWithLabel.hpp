// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "CheckBoxWithLabelItem.hpp"
#include "application-calendar/models/NewEditEventModel.hpp"

namespace gui
{
    class NewEventCheckBoxWithLabel : public CheckBoxWithLabelItem
    {
        NewEditEventModel *model = nullptr;
        app::Application *app    = nullptr;
        gui::EventDateItem *dateItem = nullptr;
        void applyCallbacks() override;

      public:
        NewEventCheckBoxWithLabel(app::Application *application,
                                  const std::string &description,
                                  NewEditEventModel *model  = nullptr);
        virtual ~NewEventCheckBoxWithLabel() override = default;

        void setConnectionToDateItem(gui::EventDateItem *item);
    };

} /* namespace gui */
