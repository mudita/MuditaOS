// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/widgets/AlarmIcon.hpp>

#include <FontManager.hpp>
#include <RawFont.hpp>
#include <apps-common/widgets/TimeSetFmtSpinner.hpp>

namespace gui
{
    AlarmIcon::AlarmIcon(Item *parent, Position x, Position y, Length w, Length h)
        : ImageBox(parent, x, y, w, h, new Image("bell_alarm_deactivated_W_M"))
    {}

    auto AlarmIcon::setStatus(Status status) noexcept -> void
    {
        switch (status) {
        case Status::ACTIVATED:
            setImage("bell_alarm_activated_W_M");
            break;
        case Status::DEACTIVATED:
            setImage("bell_alarm_deactivated_W_M");
            break;
        case Status::RINGING:
            setImage("bell_alarm_ringing_W_G");
            setMinimumSizeToFitImage();
            break;
        case Status::SNOOZE:
            setImage("bell_alarm_snooze_W_M");
            break;
        case Status::UNKNOWN:
            setImage("bell_alarm_deactivated_W_M");
            break;
        default:
            setImage("bell_alarm_deactivated_W_M");
            break;
        }

        setMinimumSizeToFitImage();
        setMaximumSize(widgetMinimumArea.w, widgetMinimumArea.h);

        informContentChanged();
    }

} /* namespace gui */
