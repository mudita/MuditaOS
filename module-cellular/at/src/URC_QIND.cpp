#include "../URC_QIND.hpp"

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
    if (tokens.size() > Val2)
    {
        return tokens[CSQ].find("csq");
    }
    return false;
}

auto QIND::csq_val() -> std::pair<int, int>
{
    if (is_csq() && tokens.size() > Val2)
    {
        return {std::stoi(tokens[Val1]), std::stoi(tokens[Val2])};
    }
    return {};
}

/// by docs ivalid csq is 99,99
auto QIND::csq_ivalid() -> std::pair<int, int>
{
    return {99, 99};
}
