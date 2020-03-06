#include "../URC_QIND.hpp"
#include <log/debug.hpp>

using namespace at::urc;

QIND::QIND(const std::string &val) : Any(val)
{
}

auto QIND::what() -> std::string
{
    return urc_name;
}

auto QIND::is_csq() -> bool
{
    if (tokens.size() > CSQ)
    {
        return tokens[CSQ].find(type_csq);
    }
    return false;
}

auto QIND::validate(enum CSQ check) -> bool
{
    try
    {
        if (is_csq() && tokens.size() > BER)
        {
            int rssi = std::stoi(tokens[RSSI]);
            int ber = std::stoi(tokens[BER]);
            LOG_DEBUG("> %d %d", rssi, ber);
            switch (check)
            {
            case RSSI:
                return (rssi % 100 % invalid_rssi_modulo) != 0;
            case BER:
                return ber != invalid_ber;
            default:
                return false;
            }
        }
    }
    catch (const std::exception &ex)
    {
        LOG_FATAL("exception: %s", ex.what());
    }
    return false;
}

auto QIND::rssiTodBm(const int rssi) -> int
{
    int ret = 0;
    if (rssi >= rssi_low && rssi <= rssi_max)
    {
        ret = rssi_low_dBm - rssi * rssi_step;
    }
    else if (rssi >= rssi_tdscmda_low && rssi <= rssi_tdscmda_max)
    {
        ret = rssi_tdscmda_low_dBm - rssi * rssi_tdscmda_step;
    }

    return ret;
}

auto QIND::getRssiDbm() -> int
{
    return rssiTodBm(getRssi());
}
