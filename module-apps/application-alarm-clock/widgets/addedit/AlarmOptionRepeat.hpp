// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "log.hpp"
#pragma once

#include <application-alarm-clock/widgets/AlarmOptionsItem.hpp>
#include <application-alarm-clock/presenter/AlarmPresenter.hpp>

namespace gui
{
    class AlarmOptionRepeat : public AlarmOptionsItem, public app::alarmClock::AlarmEventItem
    {
      public:
        AlarmOptionRepeat(app::ApplicationCommon *app,
                          AlarmOptionItemName name,
                          app::alarmClock::AlarmPresenter p,
                          std::function<void(const UTF8 &text)> mode = nullptr,
                          std::function<void()> restore              = nullptr);

        /// Will be changed - just example on how to save data
        /// sets value by presenter to model - to be set in db
        void onSave(std::shared_ptr<AlarmEventRecord> alarm);
        /// Will be changed - just example on usage how to load data
        /// loads value to UI
        void onLoad(std::shared_ptr<AlarmEventRecord> alarm);
    };

} /* namespace gui */
