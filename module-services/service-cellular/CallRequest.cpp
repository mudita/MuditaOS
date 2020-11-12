// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>
#include <memory>

#include "service-cellular/CallRequest.hpp"
#include <at/Result.hpp>
#include <at/Commands.hpp>

namespace call_request
{

    Request::Request(const std::string &data) : request(data){};
    void Request::setHandled(bool handled)
    {
        isRequestHandled = handled;
    }
    bool Request::isHandled() const noexcept
    {
        return isRequestHandled;
    }
    bool Request::checkModemResponse(const at::Result &result)
    {
        return result.code == at::Result::Code::OK;
    }

    std::string IMEIRequest::command()
    {
        return std::string(at::factory(at::AT::GET_IMEI));
    }
    void IMEIRequest::handle(CallRequestHandler &h, at::Result &result)
    {
        h.handle(*this, result);
    }

    std::unique_ptr<IMEIRequest> IMEIRequest::create(const std::string &data, GroupMatch)
    {
        return std::make_unique<IMEIRequest>(data);
    }

    std::string USSDRequest::command()
    {
        return std::string(at::factory(at::AT::CUSD_SEND) + request + ",15");
    }

    std::unique_ptr<USSDRequest> USSDRequest::create(const std::string &data, GroupMatch)
    {
        return std::make_unique<USSDRequest>(data);
    }

    void USSDRequest::handle(CallRequestHandler &h, at::Result &result)
    {
        h.handle(*this, result);
    }

    std::string CallRequest::command()
    {
        return std::string(at::factory(at::AT::ATD) + request + ";");
    }

    void CallRequest::handle(CallRequestHandler &h, at::Result &result)
    {
        h.handle(*this, result);
    }
}; // namespace call_request
