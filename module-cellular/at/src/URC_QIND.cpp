#include "../URC_QIND.hpp"
#include <log/debug.hpp>

using namespace at::urc;

QIND::QIND(const std::string &val) : Any(val)
{
}

auto QIND::what() -> std::string
{
    return "+QIND";
}

auto QIND::is_csq() -> bool
{
    if (tokens.size() > BER)
    {
        return tokens[CSQ].find("csq");
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
                return (rssi % invalid_rssi) != 0;
            case BER:
                return ber != invalid_ber;
            default:
                return false;
            }
        }
    }
    catch (std::exception *ex)
    {
        LOG_FATAL("exception: %s", ex->what());
    }
    return false;
}
