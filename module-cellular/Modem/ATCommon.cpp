#include "ATCommon.hpp"
#include <functional>
#include <log/log.hpp>
#include <string>
#include <ticks.hpp>
#include <vector>

using namespace at;

const std::string Chanel::OK = "OK";
const std::string Chanel::ERROR = "ERROR";
// const std::string Chanel::CONNECT = "CONNECT";
// const std::string Chanel::RING = "RING";
// const std::string Chanel::NO_CARRIER = "NO CARRIER";
// const std::string Chanel::NO_DIALTONE = "NO DIALTONE";
// const std::string Chanel::BUSY = "BUSY";
// const std::string Chanel::NO_ANSVER = "NO ANSWER";

Result::Code Chanel::at_check(const std::vector<std::string> &arr)
{
    Result::Code code = Result::Code::NONE;
    if (arr.size())
    {
        auto &el = arr.back();
        if (el.compare(0, OK.length(), OK) == 0)
        {
            code = Result::Code::OK;
        }
        else if (el.compare(0, ERROR.length(), ERROR) == 0)
        {
            code = Result::Code::ERROR;
        }
    }
    return code;
}

void Chanel::cmd_log(std::string cmd, const Result &result, uint32_t timeout)
{

    cmd.erase(std::remove(cmd.begin(), cmd.end(), '\r'), cmd.end());
    cmd.erase(std::remove(cmd.begin(), cmd.end(), '\n'), cmd.end());
    switch (result.code)
    {
    case Result::Code::TIMEOUT: {
        LOG_ERROR("[AT]: >%s<, timeout %d - please check the value with Quectel_EC25&EC21_AT_Commands_Manual_V1.3.pdf", cmd.c_str(), timeout);
    }
    break;
    case Result::Code::ERROR: {

        LOG_ERROR("[AT]: >%s<, >%s<", cmd.c_str(), result.response.size() ? result.response.back().c_str() : "");
    }
    break;
    default:
        LOG_DEBUG("[AT]: >%s<, >%s<", cmd.c_str(), result.response.size() ? result.response.back().c_str() : "");
        break;
    }
#if DEBUG_MODEM_OUTPUT_RESPONSE
    for (auto s : result.response)
    {
        LOG_DEBUG("[AT] > %s", s.c_str());
    }
#endif
}

class Result Chanel::cmd(const std::string cmd, uint32_t timeout, size_t rxCount)
{
    Result result;
    blockedTaskHandle = xTaskGetCurrentTaskHandle();

    cmd_init();
    cmd_send(cmd);

    uint32_t currentTime = cpp_freertos::Ticks::GetTicks();
    uint32_t timeoutNeeded = ((timeout == UINT32_MAX) ? UINT32_MAX : currentTime + timeout);
    uint32_t timeElapsed = currentTime;

    while (1)
    {
        if (timeoutNeeded != UINT32_MAX && timeElapsed >= timeoutNeeded)
        {
            result.code = Result::Code::TIMEOUT;
            break;
        }

        auto ret = ulTaskNotifyTake(pdTRUE, timeoutNeeded - timeElapsed);
        timeElapsed = cpp_freertos::Ticks::GetTicks();
        if (ret)
        {
            std::vector<std::string> ret = cmd_receive();

            result.response.insert(std::end(result.response), std::begin(ret), std::end(ret));

            result.code = at_check(ret);
            if (result.code != Result::Code::NONE)
            {
                break;
            }
            if (rxCount != 0 && result.response.size() >= rxCount)
            {
                result.code = Result::Code::TOKENS;
                break;
            }
        }
    }

    blockedTaskHandle = nullptr;
    cmd_post();
    cmd_log(cmd, result, timeout);

    return result;
}
