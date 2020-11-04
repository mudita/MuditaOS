// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallRequestFactory.hpp"

#include <re2/re2.h>

#include <at/Commands.hpp>
#include <log/log.hpp>

namespace call_request
{

    std::string IMEIRequest::process(void)
    {

        LOG_INFO("IMEIRequest::process");
        return std::string(at::factory(at::AT::GET_IMEI));
    }

    std::unique_ptr<IMEIRequest> IMEIRequest::create(const std::string &data)
    {
        return std::make_unique<IMEIRequest>(data);
    }
    std::string USSDRequest::process(void)
    {

        LOG_INFO("USSDRequest::process");
        return std::string(at::factory(at::AT::CUSD_SEND) + requestData + ",15\r");
    }

    std::string CallRequest::process(void)
    {

        LOG_INFO("CallRequest::process");
        return std::string(at::factory(at::AT::ATD) + request + ";\r");
    }

    void Factory::registerRequest(std::string regex,
                                  std::function<std::unique_ptr<IRequest>(const std::string &)> callback)
    {
        requestMap.insert(
            std::pair<std::string, std::function<std::unique_ptr<IRequest>(const std::string &)>>(regex, callback));
    }
    std::unique_ptr<IRequest> Factory::create(void)
    {

        re2::StringPiece input(request);

        if (isCallRequest()) {
            return std::make_unique<CallRequest>(request);
        }

        for (auto element : requestMap) {
            re2::RE2 reg(element.first);
            if (re2::RE2::FullMatch(input, reg)) {
                return element.second(request);
            }
        }

        return std::make_unique<USSDRequest>(request);
    }
    bool Factory::isCallRequest(void)
    {

        re2::StringPiece input(request);
        LOG_INFO("%s", request.c_str());
        re2::RE2 re("^[*].*[#]$");

        if (re2::RE2::PartialMatch(input, re)) {
            LOG_INFO("MMI Request");
            return false;
        }
        LOG_INFO("Call request");
        return true;
    }
} // namespace call_request
