#include "../URC_CUSD.hpp"
#include <log/debug.hpp>

using namespace at::urc;

CUSD::CUSD(const std::string &val) : Any(val, '\"')
{}

auto CUSD::what() -> std::string
{
    return urc_name;
}

bool CUSD::isActionNeeded(void)
{
    constexpr uint32_t actionNeeded = 2;

    utils::findAndReplaceAll(tokens[CUSD::Tokens::Status], ",", "");
    uint32_t status = 0;
    if (utils::toNumeric(tokens[CUSD::Tokens::Status], status)) {
        if (status == actionNeeded) {
            return true;
        }
    }
    return false;
}

std::string CUSD::message(void)
{
    return tokens[CUSD::Tokens::Response];
}
