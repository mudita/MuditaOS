// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "CheckBoxWithLabelItem.hpp"
#include "application-calendar/models/DateTimeModel.hpp"

namespace gui
{
    class NewEventCheckBoxWithLabel : public CheckBoxWithLabelItem
    {
        DateTimeModel *model     = nullptr;
        app::Application *app    = nullptr;
        void applyCallbacks() override;

      public:
        NewEventCheckBoxWithLabel(app::Application *application,
                                  const std::string &description,
                                  DateTimeModel *model = nullptr);
        virtual ~NewEventCheckBoxWithLabel() override = default;
    };

} /* namespace gui */
