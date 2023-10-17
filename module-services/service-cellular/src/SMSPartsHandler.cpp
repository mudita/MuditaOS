// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SMSPartsHandler.hpp"

#include <service-cellular/service-cellular/MessageConstants.hpp>
#include <log/log.hpp>

namespace cellular::internal::sms
{
    namespace
    {
        constexpr auto UCS2SingleCharacterLen = 4;
    } // anonymous namespace

    SMSPartsHandler::SMSPartsHandler(const std::string &rawMessage)
        : rawMessageUTF{UTF8(rawMessage)}, partsCount{0}, lastIndex{0}
    {
        partsCount = rawMessage.length() / msgConstants::singleMessageMaxLen;
        if ((rawMessage.length() % msgConstants::singleMessageMaxLen) != 0) {
            partsCount++;
        }
    }

    bool SMSPartsHandler::isPartsCountAboveLimit() const
    {
        return partsCount > msgConstants::maxConcatenatedCount;
    }

    bool SMSPartsHandler::isSinglePartSMS() const
    {
        return partsCount == 1;
    }

    unsigned SMSPartsHandler::getPartsCount() const
    {
        return partsCount;
    }

    const std::string SMSPartsHandler::getNextSmsPart()
    {
        auto partLength = msgConstants::singleMessageMaxLen;
        if (rawMessageUTF.length() - lastIndex < msgConstants::singleMessageMaxLen) {
            partLength = rawMessageUTF.length() - lastIndex;
        }
        if (partLength == 0) {
            LOG_ERROR("No more next parts");
            return "";
        }
        UTF8 messagePart = rawMessageUTF.substr(lastIndex, partLength);

        while (UCS2(messagePart).str().length() > (msgConstants::singleMessageMaxLen * UCS2SingleCharacterLen)) {
            partLength--;
            if (partLength <= 0) {
                // Just for safety
                LOG_ERROR("Invalid part length");
                return "";
            }
            messagePart = rawMessageUTF.substr(lastIndex, partLength);
        }
        lastIndex += messagePart.length();

        return UCS2(messagePart).str();
    }

} // namespace cellular::internal::sms
