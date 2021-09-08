// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmOptionRepeat.hpp"
#include <time/time/time_locale.hpp>

namespace gui
{
    AlarmOptionRepeat::AlarmOptionRepeat(app::ApplicationCommon *app,
                                         AlarmOptionItemName name,
                                         app::alarmClock::AlarmPresenter p,
                                         std::function<void(const UTF8 &text)> mode,
                                         std::function<void()> restore)
        : AlarmOptionsItem(app, name, std::move(mode), std::move(restore)), app::alarmClock::AlarmEventItem(p)
    {
        onSaveCallback = [&](std::shared_ptr<AlarmEventRecord> alarm) { onSave(alarm); };
        onLoadCallback = [&](std::shared_ptr<AlarmEventRecord> alarm) { onLoad(alarm); };
    }

    void AlarmOptionRepeat::onSave(std::shared_ptr<AlarmEventRecord> alarm)
    {
        typedef utils::time::Locale::Day utl;
        static int i = 0;
        if (i == 0) {
            presenter().setDays({utl::Mon, utl::Tue});
            i++;
        }
        else if (i == 1) {
            presenter().setDays({utl::Mon, utl::Tue, utl::Wed, utl::Thu, utl::Fri});
            i++;
        }
        else if (i == 2) {
            presenter().setDays({utl::Mon, utl::Tue, utl::Wed, utl::Thu, utl::Fri, utl::Sat, utl::Sun});
            i++;
        }
        else if (i == 3) {
            presenter().setDays({});
            i = 0;
        }
        LOG_DEBUG("TEXT: %s .... %s", presenter().getDescription().c_str(), alarm->rruleText.c_str());
    }

    void AlarmOptionRepeat::onLoad(std::shared_ptr<AlarmEventRecord> alarm)
    {
        LOG_INFO("loading option repeat");
        descriptionLabel->setText(presenter().getDescription());
    }
}; // namespace gui
