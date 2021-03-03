// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ATCommon.hpp"
#include <at/Cmd.hpp>
#include "time/ScopedTime.hpp"
#include <functional>
#include <log/log.hpp>
#include <string>
#include <ticks.hpp>
#include <vector>
#include <inttypes.h> // for PRIu32
#include <Utils.hpp>
#include "ATStream.hpp"
#include <at/ATFactory.hpp>

using namespace at;

const std::string Channel::OK         = "OK";
const std::string Channel::ERROR      = "ERROR";
const std::string Channel::NO_CARRIER = "NO CARRIER";
const std::string Channel::BUSY       = "BUSY";
const std::string Channel::NO_ANSWER  = "NO ANSWER";
const std::string Channel::CME_ERROR  = "+CME ERROR:";
const std::string Channel::CMS_ERROR  = "+CMS ERROR:";
// const std::string Channel::CONNECT = "CONNECT";
// const std::string Channel::RING = "RING";
// const std::string Channel::NO_DIALTONE = "NO DIALTONE";

void Channel::cmd_log(std::string cmd, const Result &result, uint32_t timeout)
{
    cmd.erase(std::remove(cmd.begin(), cmd.end(), '\r'), cmd.end());
    cmd.erase(std::remove(cmd.begin(), cmd.end(), '\n'), cmd.end());
    switch (result.code) {
    case Result::Code::TIMEOUT: {
        LOG_INFO("[AT]: >%s<, timeout %" PRIu32
                 " - please check the value with Quectel_EC25&EC21_AT_Commands_Manual_V1.3.pdf",
                 cmd.c_str(),
                 timeout);
    } break;
    case Result::Code::ERROR: {
        LOG_INFO("[AT]: >%s<, >%s<", cmd.c_str(), result.response.size() ? result.response.back().c_str() : "");
    } break;
    default:
        LOG_INFO("[AT]: >%s<, >%s<", cmd.c_str(), result.response.size() ? result.response.back().c_str() : "");
        break;
    }
#if DEBUG_MODEM_OUTPUT_RESPONSE
    for (auto s : result.response) {
        LOG_DEBUG("[AT] > %s", s.c_str());
    }
#endif
}

std::string Channel::formatCommand(const std::string &cmd) const
{
    bool isTerminatorValid = std::find(validTerm.begin(), validTerm.end(), cmd.back()) != validTerm.end();
    if (isTerminatorValid) {
        return cmd;
    }
    return cmd + cmdSeparator;
}

Result Channel::cmd(const std::string &cmd, std::chrono::milliseconds timeout, size_t rxCount)
{
    Result result;
    blockedTaskHandle = xTaskGetCurrentTaskHandle();

    cmd_init();
    std::string cmdFixed = formatCommand(cmd);

    cmd_send(cmdFixed);

    uint32_t currentTime   = cpp_freertos::Ticks::GetTicks();
    uint32_t timeoutNeeded = ((timeout.count() == UINT32_MAX) ? UINT32_MAX : currentTime + timeout.count());
    uint32_t timeElapsed   = currentTime;

    ATStream atStream(rxCount);

    while (true) {
        if (timeoutNeeded != UINT32_MAX && timeElapsed >= timeoutNeeded) {
            result.code = Result::Code::TIMEOUT;
            break;
        }

        auto taskUnlocked = ulTaskNotifyTake(pdTRUE, timeoutNeeded - timeElapsed);
        timeElapsed       = cpp_freertos::Ticks::GetTicks();

        if (taskUnlocked) {
            atStream.write(cmd_receive());

            if (atStream.isReady()) {
                result = atStream.getResult();
                break;
            }
        }
    }

    blockedTaskHandle = nullptr;
    cmd_post();
    cmd_log(cmdFixed, result, timeout.count());

    return result;
}

auto Channel::cmd(const at::Cmd &at) -> Result
{
    auto time = utils::time::Scoped("Time to run at command" + at.getCmd());
    return cmd(at.getCmd(), at.getTimeout());
}

auto Channel::cmd(const at::AT &at) -> Result
{
    auto cmd  = at::factory(at);
    auto time = utils::time::Scoped("Time to run at command" + cmd.getCmd());
    return this->cmd(cmd);
}
