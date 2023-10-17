// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "VolteHandlerImpl.hpp"

namespace cellular::internal
{
    bool ModemResponseParserImpl::operator()(QcfgImsResult const &response, bool requestedState) const
    {
        using namespace at::response::qcfg_ims;

        std::pair<IMSState, VoLTEIMSState> parsed;
        if (!at::response::parseQCFG_IMS(response, parsed)) {
            throw std::runtime_error("[VoLTE] unable to parse modem's response to QCFG with IMS");
        }

        auto const &ims          = parsed.first;
        bool stateMatchesRequest = (ims != IMSState::Disable) == requestedState;
        LOG_INFO("[VoLTE] current IMS state: %s which %s expectation",
                 magic_enum::enum_name(ims).data(),
                 ((stateMatchesRequest) ? "matches" : "doesn't match"));
        return stateMatchesRequest;
    }
} // namespace cellular::internal
