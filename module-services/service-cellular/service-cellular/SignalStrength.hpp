// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <EventStore.hpp>

#include <limits>
class SignalStrength
{
    // <rssi>   0           -113dBm or less
    //          1           -111dBm
    //          2...30      -109dBm...-53dBm
    //          31          -51dBm or greater
    //          99          Not known or not detectable
    //          100         -116dBm or less
    //          101         -115dBm
    //          102..190    -114dBm...-26dBm
    //          191         -25dBm or greater
    //          199         Not known or not detectable
    static const auto rssi_low     = 0;
    static const auto rssi_max     = 31;
    static const auto rssi_low_dBm = -113;
    static const auto rssi_max_dBm = -51;
    static const auto rssi_invalid = 99;
    static const auto rssi_step    = (rssi_low_dBm - rssi_max_dBm) / (rssi_max - rssi_low);

    static const auto rssi_tdscmda_low     = 100;
    static const auto rssi_tdscmda_max     = 191;
    static const auto rssi_tdscmda_invalid = 199;
    static const auto rssi_tdscmda_low_dBm = -116;
    static const auto rssi_tdscmda_max_dBm = -25;
    static const auto rssi_tdscmda_step = (rssi_tdscmda_low_dBm - rssi_tdscmda_max_dBm) / (rssi_tdscmda_max - rssi_low);

    static const auto rssidBm_invalid          = 0;
    static const auto rssidBm_four_bar_margin  = -60;
    static const auto rssidBm_three_bar_margin = -75;
    static const auto rssidBm_two_bar_margin   = -90;
    static const auto rssidBm_one_bar_margin   = -100;

  public:
    SignalStrength(int rssi);
    ~SignalStrength() = default;
    SignalStrength(Store::SignalStrength);

    Store::SignalStrength data;

    void setRssi(int rssi);
    bool isValid()
    {
        return data.rssidBm != rssidBm_invalid;
    };

    /// @return 0 - if invalid
    static int rssiTodBm(const int rssi);
    static Store::RssiBar rssidBmToBar(const int rssidBm);
};
