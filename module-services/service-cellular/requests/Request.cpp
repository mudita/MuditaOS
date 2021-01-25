// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>
#include <memory>

#include <at/Result.hpp>
#include <at/Commands.hpp>

#include "service-cellular/requests/Request.hpp"

namespace cellular
{
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
        return result.getStatusCode() == at::Result::StatusCode::OK;
    }

    bool Request::isValid() const noexcept
    {
        return true;
    }

    std::string Request::buildCommand(at::AT atCommand,
                                      const std::vector<commandBuilderFunc> &builderFunctions,
                                      bool trim) const
    {
        if (!isValid()) {
            return std::string();
        }

        std::string cmd(at::factory(atCommand));

        auto formatFirst = true;
        for (auto &cmdPart : builderFunctions) {
            auto partStr = cmdPart();
            if (partStr.empty() && trim) {
                break;
            }
            cmd.append(formatFirst ? partStr : "," + partStr);
            formatFirst = false;
        }

        return cmd;
    }
}; // namespace cellular
