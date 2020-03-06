#include "EventStore.hpp"
#include <log/log.hpp>

namespace Store
{
    // if it grows bigger than these few variables - consider moving it to ram with i.e.
    // delayed construction singletone
    Battery battery;

    const Battery &Battery::get()
    {
        return battery;
    }

    Battery &Battery::modify()
    {
        return battery;
    }

    GSM *GSM::ptr = nullptr;

    GSM *GSM::get()
    {
        if (!ptr)
        {
            ptr = new GSM();
        }
        return ptr;
    }

    SignalStrength *SignalStrength::ptr = nullptr;

    SignalStrength *SignalStrength::get()
    {
        if (!ptr)
        {
            ptr = new SignalStrength();
        }
        return ptr;
    }

    SignalStrength::~SignalStrength()
    {
        delete ptr;
    }

    int SignalStrength::rssiTodBm(const int rssi)
    {
        int ret = 0;
        if (rssi >= rssi_low && rssi <= rssi_max)
        {
            ret = rssi_low_dBm - rssi * rssi_step;
        }
        else if (rssi >= rssi_tdscmda_low && rssi <= rssi_tdscmda_max)
        {
            LOG_WARN("TD-SCDMA rssi range - signal strength indicator not fully supported");
            ret = rssi_tdscmda_low_dBm - rssi * rssi_tdscmda_step;
        }

        return ret;
    }

    SignalStrength::RssiBar SignalStrength::rssidBmToBar(const int rssidBm)
    {
        auto newStrength = RssiBar::zero;
        if (rssidBm == 0)
            newStrength = RssiBar::invalid;
        else if (rssidBm >= rssidBm_five_bar_margin)
            newStrength = RssiBar::five;
        else if (rssidBm >= rssidBm_four_bar_margin)
            newStrength = RssiBar::four;
        else if (rssidBm >= rssidBm_three_bar_margin)
            newStrength = RssiBar::three;
        else if (rssidBm >= rssidBm_two_bar_margin)
            newStrength = RssiBar::two;
        else if (rssidBm >= rssidBm_one_bar_margin)
            newStrength = RssiBar::one;
        else
            newStrength = RssiBar::zero;

        return newStrength;
    }

    void SignalStrength::setRssi(const int rssi)
    {
        if (auto rssidBm = rssiTodBm(rssi) != rssidBm_invalid)
        {
            this->rssidBm = rssiTodBm(rssi);
            this->rssiBar = rssidBmToBar(rssidBm);
            this->rssi = rssi;
            LOG_DEBUG("Signal strength set to rssi %d : %d dBm : %d bars", this->rssi, this->rssidBm, static_cast<int>(this->rssiBar));
        }
        else
        {
            LOG_WARN("Invalid rssi value - skipped");
        }
    }

}; // namespace Store
