// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ATCommon.hpp"
#include "ResultParser.hpp"

#include <functional>
#include <log/log.hpp>
#include <string>
#include <ticks.hpp>
#include <vector>
#include <inttypes.h> // for PRIu32
#include <Utils.hpp>

using namespace at;

const std::string Chanel::OK         = "OK";
const std::string Chanel::ERROR      = "ERROR";
const std::string Chanel::NO_CARRIER = "NO CARRIER";
const std::string Chanel::BUSY       = "BUSY";
const std::string Chanel::NO_ANSWER  = "NO ANSWER";
const std::string Chanel::CME_ERROR  = "+CME ERROR:";
const std::string Chanel::CMS_ERROR  = "+CMS ERROR:";
// const std::string Chanel::CONNECT = "CONNECT";
// const std::string Chanel::RING = "RING";
// const std::string Chanel::NO_DIALTONE = "NO DIALTONE";


void Chanel::cmd_log(std::string cmd, const Result &result, uint32_t timeout)
{
    cmd.erase(std::remove(cmd.begin(), cmd.end(), '\r'), cmd.end());
    cmd.erase(std::remove(cmd.begin(), cmd.end(), '\n'), cmd.end());
    switch (result.getStatusCode()) {
    case Result::StatusCode::TIMEOUT: {
        LOG_ERROR("[AT]: >%s<, timeout %" PRIu32
                  " - please check the value with Quectel_EC25&EC21_AT_Commands_Manual_V1.3.pdf",
                  cmd.c_str(),
                  timeout);
    } break;
    case Result::StatusCode::ERROR: {

        LOG_ERROR(
            "[AT]: >%s<, >%s<", cmd.c_str(), result.getResponse().size() ? result.getResponse().back().c_str() : "");
    } break;
    default:
        LOG_DEBUG(
            "[AT]: >%s<, >%s<", cmd.c_str(), result.getResponse().size() ? result.getResponse().back().c_str() : "");
        break;
    }
#if DEBUG_MODEM_OUTPUT_RESPONSE
    for (auto s : result.getResponse()) {
        LOG_DEBUG("[AT] > %s", s.c_str());
    }
#endif
}

std::string Chanel::formatCommand(const std::string &cmd) const
{
    bool isTerminatorValid = std::find(validTerm.begin(), validTerm.end(), cmd.back()) != validTerm.end();
    if (isTerminatorValid) {
        return cmd;
    }
    return cmd + cmdSeparator;
}

std::shared_ptr<Result> Chanel::cmd(const std::string cmd, uint32_t timeout, size_t rxCount)
{
    ResultParser resultParser;

    blockedTaskHandle = xTaskGetCurrentTaskHandle();

    cmd_init();
    std::string cmdFixed = formatCommand(cmd);
    cmd_send(cmdFixed);

    uint32_t currentTime   = cpp_freertos::Ticks::GetTicks();
    uint32_t timeoutNeeded = ((timeout == UINT32_MAX) ? UINT32_MAX : currentTime + timeout);
    uint32_t timeElapsed   = currentTime;

    while (1) {
        if (timeoutNeeded != UINT32_MAX && timeElapsed >= timeoutNeeded) {
            resultParser.setStatusCode(Result::StatusCode::TIMEOUT);
            break;
        }

        auto notify = ulTaskNotifyTake(pdTRUE, timeoutNeeded - timeElapsed);
        timeElapsed = cpp_freertos::Ticks::GetTicks();
        if (notify) {
            std::vector<std::string> ret = cmd_receive();

            if (resultParser.appendPartialResult(ret)) {
                break;
            };

            if (rxCount != 0 && resultParser.getResponse().size() >= rxCount) {
                resultParser.setStatusCode(Result::StatusCode::TOKENS);
                break;
            }
        }
    }

    blockedTaskHandle = nullptr;
    cmd_post();

    auto result = resultParser.parse();
    cmd_log(cmdFixed, *result.get(), timeout);

    return result;
}

auto Chanel::cmd(at::Cmd &at) -> std::shared_ptr<Result>
{
    at.last.requested = cpp_freertos::Ticks::GetTicks();
    auto result       = this->cmd(at.cmd, at.timeout);
    at.last.response  = cpp_freertos::Ticks::GetTicks();
    at.last.status    = result->getStatusCode();
    return result;
}

auto Chanel::cmd(const at::AT at) -> std::shared_ptr<Result>
{
    auto cmd = at::factory(at);
    return this->cmd(cmd);
}
