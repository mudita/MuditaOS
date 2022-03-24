// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <cstring>
#include "ATStream.hpp"
#include "ATCommon.hpp"
#include <map>

namespace at
{

    Result::Code ATStream::parseState(const std::string &state, uint32_t &errcode)
    {
        const std::map<std::string, Result::Code> convert_map = {
            {Channel::OK, Result::Code::OK},
            {Channel::ERROR, Result::Code::ERROR},
            {Channel::CME_ERROR, Result::Code::CME_ERROR},
            {Channel::CMS_ERROR, Result::Code::CMS_ERROR},

        };

        auto it = convert_map.find(state);
        if (it != convert_map.end()) {
            if ((it->second == Result::Code::CME_ERROR) || (it->second == Result::Code::CMS_ERROR)) {
                return Result::Code::PARSING_ERROR;
            }
            return it->second;
        }

        auto cmxErrorTest = state.substr(0, Channel::CME_ERROR.length()); /// same length for CMS

        it = convert_map.find(cmxErrorTest);
        if ((it != convert_map.end()) &&
            ((it->second == Result::Code::CME_ERROR) || (it->second == Result::Code::CMS_ERROR))) {
            auto serr =
                utils::trim(state.substr(Channel::CME_ERROR.length(), state.length() - Channel::CME_ERROR.length()));
            if (serr.length() == 0) {
                return Result::Code::PARSING_ERROR;
            }
            int parsedVal = 0;
            auto ret      = utils::toNumeric(serr, parsedVal);
            if (!ret) {
                return Result::Code::PARSING_ERROR;
            }
            errcode = parsedVal;
            return it->second;
        }

        return Result::Code::NONE;
    }

    void ATStream::checkError()
    {
        if (result.code == Result::Code::CME_ERROR) {
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
        }

        if (result.code == Result::Code::CMS_ERROR) {
            result.code =
                Result::Code::ERROR; // setup error but in this case error from +CME ERROR with valid errorCode

            auto atmp_ec = magic_enum::enum_cast<NetworkErrorCode>(errcode);

            if (atmp_ec.has_value()) {
                LOG_ERROR("%s", utils::enumToString(atmp_ec.value()).c_str());
                result.errorCode = atmp_ec.value();
            }
            else {
                LOG_ERROR("Unknown CMS error code %" PRIu32, errcode);
                result.errorCode = at::NetworkErrorCode::Unknown;
            }
        }
    }

    bool ATStream::checkATBegin()
    {
        const auto delimiterLength = std::strlen(at::delimiter);
        auto pos                   = atBuffer.find(at::delimiter, delimiterLength);
        if (pos != std::string::npos) {
            beginChecked   = true;
            std::string rr = atBuffer.substr(delimiterLength, pos - delimiterLength);
            auto code      = parseState(rr, errcode);
            if (code != Result::Code::NONE) {
                result.code = code;
                return true;
            }
        }
        return false;
    }

    bool ATStream::checkATEnd()
    {
        auto pos = atBuffer.rfind(at::delimiter);
        const auto delimiterLength = std::strlen(at::delimiter);
        if (pos != std::string::npos) {
            auto pos2 = atBuffer.rfind(at::delimiter, pos - delimiterLength);
            if (pos2 != std::string::npos) {
                std::string rr = atBuffer.substr(pos2 + delimiterLength, pos - pos2 - delimiterLength);
                auto code = parseState(rr, errcode);
                if (code != Result::Code::NONE) {
                    result.code = code;
                    return true;
                }
            }
        }

        return false;
    }

    void ATStream::countLines()
    {
        if (rxCount != 0) {
            auto pos = atBuffer.find(at::delimiter, lastPos);
            const auto delimiterLength = std::strlen(at::delimiter);
            while (pos != std::string::npos) {
                /// do not count empty lines, see implementation of utils:split
                if ((lastPos) != pos) {
                    lineCounter++;
                }
                lastPos = pos + delimiterLength;
                pos     = atBuffer.find(at::delimiter, lastPos);
            }
        }
    }

    bool ATStream::parse()
    {
        /// check for return at the begin eg <cr<lf>X<cr><lf>
        if ((!beginChecked) && (atBuffer.length() > minATCmdRet) && (atBuffer[0] == '\r') && (atBuffer[1] == '\n')) {
            if (checkATBegin()) {
                atAtTheBegin = true;
                /**
                 * This is a compromise between the certainty of getting all the elements and the waiting time.
                 * In case if we always waited, the waiting time would be equal timeout.
                 * In this case, we wait for a specified number of commands,
                 * the wrong one may result in returning incorrect / incomplete values.
                 * It only applies to a group of specific commands like AT + QPING, in which the result (OK)
                 * is returned at the beginning, followed by information. This could happen only if we not fit
                 * in one return of cmdReceive return.
                 */
                if (rxCount == 0) {
                    return true;
                }
            }
        }
        countLines();
        /**
         * Check for pattern <cr><lf>XXX<cr><lf> at the end or check counted lines
         * XXX must be one of AT return codes
         */
        if (atAtTheBegin) {
            if (rxCount <= (lineCounter)) {
                return true;
            }
        }
        else {
            if (checkATEnd()) {
                return true;
            }
        }

        return false;
    }

    /**
     *
     * @param buffer
     * @return false if could not write (mean ready). Reset to write again
     */
    bool ATStream::write(const std::string &buffer)
    {
        if (isATReady) {
            return false;
        }
        atBuffer += buffer;
        if (parse()) {
            isATReady = true;

            auto fullRet = utils::split(atBuffer, at::delimiter);
            result.response.insert(std::end(result.response), std::begin(fullRet), std::end(fullRet));

            checkError();

            if (rxCount != 0 && result.response.size() > rxCount) {
                result.code = Result::Code::TOKENS;
            }
            return false;
        }
        else {

            return true;
        }
    }

    void ATStream::reset()
    {
        result.code = Result::Code::NONE;
        result.response.clear();
        result.errorCode = at::EquipmentErrorCode::NoInformation;
        atAtTheBegin     = false;
        beginChecked     = false;
        lastPos          = 0;
        lineCounter      = 0;
        atBuffer         = {};
        isATReady        = false;
        errcode          = 0;
    }
} // namespace at
