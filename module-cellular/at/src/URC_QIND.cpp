#include "../URC_QIND.hpp"
#include <segger/log/debug.hpp>

using namespace at::urc;

QIND::QIND(const std::string &val) : Any(val)
{}

auto QIND::what() -> std::string
{
    return urc_name;
}

auto QIND::is_csq() -> bool
{
    if (tokens.size() > CSQ) {
        return tokens[CSQ].find(type_csq) != std::string::npos;
    }
    return false;
}

auto QIND::validate(enum CSQ check) -> bool
{
    try {
        if (is_csq() && tokens.size() > BER) {
            int rssi = std::stoi(tokens[RSSI]);
            int ber  = std::stoi(tokens[BER]);
            LOG_DEBUG("> %d %d", rssi, ber);
            switch (check) {
            case RSSI:
                return (rssi % 100 % invalid_rssi_modulo) != 0;
            case BER:
                return ber != invalid_ber;
            default:
                return false;
            }
        }
    }
    catch (const std::exception &ex) {
        LOG_FATAL("exception: %s", ex.what());
    }
    return false;
}
