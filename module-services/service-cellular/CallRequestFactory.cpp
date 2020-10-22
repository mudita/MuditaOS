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
        return std::string(at::factory(at::AT::AT));
    }

    std::string USSDRequest::process(void)
    {

        LOG_INFO("USSDRequest::process");
        return std::string(at::factory(at::AT::AT));
    }

    std::string CallRequest::process(void)
    {

        LOG_INFO("CallRequest::process");
        return std::string(at::factory(at::AT::ATD) + request + ";\r");
    }

    IRequest *Factory::create(void)
    {
        re2::StringPiece input(request);
        re2::RE2 re("(\\*#06#)");

        if (isCallRequest()) {
            return new CallRequest(request);
        }
        else {
            if (re2::RE2::FullMatch(input, re)) {
                return new IMEIRequest(request);
            }
        }
        return new USSDRequest(request);
    }
    bool Factory::isCallRequest(void)
    {

        re2::StringPiece input(request);
        LOG_INFO("%s", request.c_str());
        re2::RE2 re("^[*].*[#]$");
        //	re2::RE2 re("[\\*.#]");

        if (re2::RE2::PartialMatch(input, re)) {
            LOG_INFO("MMI Request");
            return false;
        }
        LOG_INFO("Call request");
        return true;
    }
} // namespace call_request
