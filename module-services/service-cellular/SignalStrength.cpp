// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-cellular/SignalStrength.hpp"
#include <log/log.hpp>

SignalStrength::SignalStrength(int rssi)
{
    setRssi(rssi);
}

int SignalStrength::rssiTodBm(const int rssi)
{
    int ret = rssidBm_invalid;
    if (rssi == rssi_invalid || rssi == rssi_tdscmda_invalid) {
        return ret;
    }

    if (rssi >= rssi_low && rssi <= rssi_max) {
        ret = rssi_low_dBm - rssi * rssi_step;
    }
    else if (rssi >= rssi_tdscmda_low && rssi <= rssi_tdscmda_max) {
        LOG_WARN("TD-SCDMA rssi range - signal strength indicator not fully supported");
        ret = rssi_tdscmda_low_dBm - rssi * rssi_tdscmda_step;
    }

    return ret;
}

Store::RssiBar SignalStrength::rssidBmToBar(const int rssidBm)
{
    using namespace Store;

    if (rssidBm == rssidBm_invalid) {
        return RssiBar::zero;
    }
    else if (rssidBm >= rssidBm_four_bar_margin) {
        return RssiBar::four;
    }
    else if (rssidBm >= rssidBm_three_bar_margin) {
        return RssiBar::three;
    }
    else if (rssidBm >= rssidBm_two_bar_margin) {
        return RssiBar::two;
    }
    else if (rssidBm >= rssidBm_one_bar_margin) {
        return RssiBar::one;
    }
    else {
        return RssiBar::zero;
    }
}

void SignalStrength::setRssi(const int rssi)
{
    data.rssi    = rssi;
    data.rssidBm = rssiTodBm(data.rssi);
    data.rssiBar = rssidBmToBar(data.rssidBm);
}
