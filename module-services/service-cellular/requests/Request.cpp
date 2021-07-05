// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string>
#include <memory>

#include <at/Result.hpp>
#include <at/Commands.hpp>
#include <at/ATFactory.hpp>

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
        return result.code == at::Result::Code::OK;
    }

    bool Request::isValid() const noexcept
    {
        return true;
    }

    at::Cmd Request::buildCommand(at::AT atCommand,
                                  const std::vector<commandBuilderFunc> &builderFunctions,
                                  bool trim) const
    {
        if (!isValid()) {
            return at::Cmd("");
        }

        auto cmd = at::factory(atCommand);

        auto formatFirst = true;
        for (auto &cmdPart : builderFunctions) {
            auto partStr = cmdPart();
            if (partStr.empty() && trim) {
                break;
            }
            cmd         = cmd + (formatFirst ? partStr : "," + partStr);
            formatFirst = false;
        }

        return cmd;
    }
}; // namespace cellular
