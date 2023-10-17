// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SMSParser.hpp"

#include <log/log.hpp>
#include <ucs2/UCS2.hpp>

#include <algorithm>
#include <ctime>
#include <map>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace SMSParser
{
    namespace
    {
        static constexpr auto SMSReadMsgName        = "QCMGR";
        static constexpr auto SingleMessageLength   = 5;
        static constexpr auto MultipleMessageLength = 8;

        std::unordered_map<std::uint16_t, std::map<std::uint8_t, std::string>> concatenatedMessage;
        std::string message;
        UTF8 number;
    } // namespace

    bool parse(const std::string *smsData)
    {
        bool parsed = false;

        if (smsData[0].find(SMSReadMsgName) == std::string::npos) {
            return parsed;
        }

        std::istringstream ss(smsData[0]);
        std::string token;
        std::vector<std::string> tokens;
        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        /*
         * tokens:
         * [0] - +QCMGR
         * [1] - sender number
         * [2] - none
         * [3] - date YY/MM/DD
         * [4] - hour HH/MM/SS/timezone
         * concatenaded messages
         * [5] - unique concatenated message id (range: 0-65535)
         * [6] - current message number
         * [7] - total messages count
         */
        tokens[1].erase(std::remove(tokens[1].begin(), tokens[1].end(), '\"'), tokens[1].end());
        number = UCS2(tokens[1]).toUTF8();

        // parse date
        tokens[3].erase(std::remove(tokens[3].begin(), tokens[3].end(), '\"'), tokens[3].end());

        if (tokens.size() == SingleMessageLength) {
            LOG_DEBUG("Single message");
            message = smsData[1];
            parsed  = true;
        }
        else if (tokens.size() == MultipleMessageLength) {
            std::uint32_t smsId   = 0;
            std::uint32_t current = 0;
            std::uint32_t total   = 0;
            try {
                smsId   = std::stoi(tokens[5]);
                current = std::stoi(tokens[6]);
                total   = std::stoi(tokens[7]);
            }
            catch (const std::exception &e) {
                LOG_ERROR("Parse SMS error %s", e.what());
                parsed = false;
                return parsed;
            }
            concatenatedMessage[smsId][current] = smsData[1];
            auto receivedConcatenatedMessages   = concatenatedMessage[smsId].size();
            LOG_DEBUG("Concatenated message uid: %" PRIu32 " rcv: %zu [%" PRIu32 "/%" PRIu32 "]",
                      smsId,
                      receivedConcatenatedMessages,
                      current,
                      total);
            if (receivedConcatenatedMessages == total) {
                message.clear();
                for (const auto &[_, smsMessageCat] : concatenatedMessage[smsId]) {
                    message += smsMessageCat;
                }
                concatenatedMessage.erase(smsId);
                parsed = true;
            }
        }
        return parsed;
    }

    UTF8 getNumber()
    {
        return number;
    }

    std::string getMessage()
    {
        return message;
    }

    time_t getTime()
    {
        return std::time(nullptr);
    }

} // namespace SMSParser
