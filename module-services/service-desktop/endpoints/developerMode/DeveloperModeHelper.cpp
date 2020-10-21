// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DeveloperModeHelper.hpp"

#include "ParserUtils.hpp" // for parserFSM

namespace parserFSM
{
    class Context;
} // namespace parserFSM

using namespace parserFSM;
auto DeveloperModeHelper::processGetRequest(Context &context) -> sys::ReturnCodes
{
    return sys::ReturnCodes::Unresolved;
}
