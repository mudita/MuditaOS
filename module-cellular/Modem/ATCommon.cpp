// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ATCommon.hpp"
#include "time/ScopedTime.hpp"
#include <functional>
#include <log/log.hpp>
#include <string>
#include <ticks.hpp>
#include <vector>
#include <inttypes.h> // for PRIu32
#include <Utils.hpp>

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

Result::Code Channel::at_check(const std::vector<std::string> &arr)
{
    if (arr.size()) {
        for (auto el : arr) {
            if (el.compare(0, OK.length(), OK) == 0) {
                return Result::Code::OK;
            }
            else if (el.compare(0, ERROR.length(), ERROR) == 0) {
                return Result::Code::ERROR;
            }
        }
    }
    return Result::Code::NONE;
}

bool Channel::at_check_cmx_error(const std::string &CMX, const std::vector<std::string> &arr, uint32_t &errcode)
{
    if (arr.size()) {
        for (auto cmxerr : arr) {
            if (cmxerr.compare(0, CMX.length(), CMX) == 0) {
                auto serr     = utils::trim(cmxerr.substr(CMX.length(), cmxerr.length() - CMX.length()));
                int parsedVal = 0;
                auto ret      = utils::toNumeric(serr, parsedVal);
                errcode       = parsedVal;
                return ret;
            }
        }
    }
    return false;
}

void Channel::cmd_log(std::string cmd, const Result &result, uint32_t timeout)
{
    cmd.erase(std::remove(cmd.begin(), cmd.end(), '\r'), cmd.end());
    cmd.erase(std::remove(cmd.begin(), cmd.end(), '\n'), cmd.end());
    switch (result.code) {
    case Result::Code::TIMEOUT: {
        LOG_ERROR("[AT]: >%s<, timeout %" PRIu32
                  " - please check the value with Quectel_EC25&EC21_AT_Commands_Manual_V1.3.pdf",
                  cmd.c_str(),
                  timeout);
    } break;
    case Result::Code::ERROR: {

        LOG_ERROR("[AT]: >%s<, >%s<", cmd.c_str(), result.response.size() ? result.response.back().c_str() : "");
    } break;
    default:
        LOG_DEBUG("[AT]: >%s<, >%s<", cmd.c_str(), result.response.size() ? result.response.back().c_str() : "");
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

    while (true) {
        if (timeoutNeeded != UINT32_MAX && timeElapsed >= timeoutNeeded) {
            result.code = Result::Code::TIMEOUT;
            break;
        }

        auto ret    = ulTaskNotifyTake(pdTRUE, timeoutNeeded - timeElapsed);
        timeElapsed = cpp_freertos::Ticks::GetTicks();
        if (ret != 0u) {
            std::vector<std::string> ret = cmd_receive();

            result.response.insert(std::end(result.response), std::begin(ret), std::end(ret));

            uint32_t errcode = 0;
            if (at_check_cmx_error(CME_ERROR, ret, errcode)) {
                result.code =
                    Result::Code::ERROR; // setup error but in this case error from +CME ERROR with valid errorCode
                auto tmp_ec = magic_enum::enum_cast<EquipmentErrorCode>(errcode);
                if (tmp_ec.has_value()) {
                    LOG_ERROR("%s", utils::enumToString(tmp_ec.value()).c_str());
                    result.errorCode = tmp_ec.value();
                }
                else {
                    LOG_ERROR("Unknow CME error code %" PRIu32, errcode);
                    result.errorCode = at::EquipmentErrorCode::Unknown;
                }
                break;
            }

            if (at_check_cmx_error(CMS_ERROR, ret, errcode)) {
                result.code =
                    Result::Code::ERROR; // setup error but in this case error from +CME ERROR with valid errorCode

                auto atmp_ec = magic_enum::enum_cast<NetworkErrorCode>(errcode);

                if (atmp_ec.has_value()) {
                    LOG_ERROR("%s", utils::enumToString(atmp_ec.value()).c_str());
                    result.errorCode = atmp_ec.value();
                }
                else {
                    LOG_ERROR("Unknow CMS error code %" PRIu32, errcode);
                    result.errorCode = at::NetworkErrorCode::Unknown;
                }
                break;
            }

            result.code = at_check(ret);
            if (result.code != Result::Code::NONE) {
                break;
            }
            if (rxCount != 0 && result.response.size() >= rxCount) {
                result.code = Result::Code::TOKENS;
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
    auto cmd = at::factory(at);
    auto time = utils::time::Scoped("Time to run at command" + cmd.getCmd());
    return this->cmd(cmd);
}
