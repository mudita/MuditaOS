#include "../URC_CTZE.hpp"
#include <log/debug.hpp>

using namespace at::urc;

CTZE::CTZE(const std::string &val) : Any(val)
{}

auto CTZE::what() -> std::string
{
    return urc_name;
}

struct tm CTZE::getTimeInfo(void)
{
    struct tm timeinfo;
    std::string dateTimeStr(tokens[tokensDescription::date] + "," + tokens[tokensDescription::time]);
    utils::findAndReplaceAll(dateTimeStr, "\"", "");

    std::stringstream stream(dateTimeStr);
    stream >> std::get_time(&timeinfo, "%Y/%m/%d,%H:%M:%S");
    constexpr uint32_t signLenght  = 1;
    constexpr uint32_t valueLength = 2;
    constexpr uint32_t signPos     = 0;

    utils::findAndReplaceAll(tokens[tokensDescription::GMTdiference], "\"", "");
    auto sign  = tokens[tokensDescription::GMTdiference].substr(signPos, signLenght);
    auto value = tokens[tokensDescription::GMTdiference].substr(signLenght, valueLength);

    uint32_t offset;
    if (utils::toNumeric(value, offset)) {
        constexpr uint32_t quatersInHour = 4;
        offset /= quatersInHour;
        if (sign == "+") {
            timeinfo.tm_hour += offset;
        }
        else if (sign == "-") {
            timeinfo.tm_hour -= offset;
        }
    }

    return timeinfo;
}
