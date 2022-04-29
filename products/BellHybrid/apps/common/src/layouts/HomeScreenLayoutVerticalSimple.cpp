// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "layouts/HomeScreenLayoutVerticalSimple.hpp"
#include "data/BellMainStyle.hpp"
#include "widgets/BellBattery.hpp"
#include "widgets/DuoHBox.hpp"
#include "widgets/SnoozeTimer.hpp"

#include <apps-common/actions/AlarmRingingData.hpp>
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Icon.hpp>
#include <gui/widgets/text/TextFixedSize.hpp>
#include <gui/widgets/Style.hpp>
#include <time/time_constants.hpp>
#include <widgets/AlarmIcon.hpp>
#include <widgets/AlarmSetSpinner.hpp>
#include <widgets/ClockVertical.hpp>

namespace gui
{
    HomeScreenLayoutVerticalSimple::HomeScreenLayoutVerticalSimple(std::string name)
        : HomeScreenLayoutVertical(std::move(name))
    {
        using namespace bellMainStyle;

        leftBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        leftBox->addWidget(battery);
        battery->informContentChanged();

        rightBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        rightBox->addWidget(alarmMainIcon);
        alarmMainIcon->informContentChanged();

        leftBox->resizeItems();
        rightBox->resizeItems();
    }
}; // namespace gui
