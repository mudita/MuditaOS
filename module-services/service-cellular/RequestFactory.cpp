// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-cellular/RequestFactory.hpp"

#include <re2/re2.h>

#include <at/Commands.hpp>
#include <log/log.hpp>

#include "service-cellular/requests/CallRequest.hpp"
#include "service-cellular/requests/SupplementaryServicesRequest.hpp"
#include "service-cellular/requests/PasswordRegistrationRequest.hpp"
#include "service-cellular/requests/PinChangeRequest.hpp"
#include "service-cellular/requests/ImeiRequest.hpp"
#include "service-cellular/requests/UssdRequest.hpp"
#include "service-cellular/requests/RejectRequest.hpp"

#include <at/ATFactory.hpp>
#include <EventStore.hpp>
#include <cmd/QECCNUM.hpp>

namespace cellular
{
    RequestFactory::RequestFactory(const std::string &data,
                                   at::BaseChannel &channel,
                                   cellular::api::CallMode callMode,
                                   bool simInserted)
        : request(data), channel(channel), callMode(callMode), simInserted(simInserted)
    {
        registerRequest(ImeiRegex, ImeiRequest::create);
        registerRequest(PasswordRegistrationRegex, PasswordRegistrationRequest::create);
        registerRequest(PinChangeRegex, PinChangeRequest::create);
        registerRequest(SupplementaryServicesRegex, SupplementaryServicesRequest::create);
        /*It have to be last check due to 3GPP TS 22.030*/
        registerRequest(UssdRegex, UssdRequest::create);
    }

    void RequestFactory::registerRequest(std::string regex, CreateCallback callback)
    {
        requestMap.emplace_back(std::make_pair(regex, callback));
    }

    std::unique_ptr<IRequest> RequestFactory::emergencyCheck()
    {
        at::cmd::QECCNUM cmd;
        auto qeccnumResult   = channel.cmd(cmd);
        auto qeccnumResponse = cmd.parseQECCNUM(qeccnumResult);

        auto isSimEmergency =
            std::find(qeccnumResponse.eccNumbersSim.begin(), qeccnumResponse.eccNumbersSim.end(), request) !=
            qeccnumResponse.eccNumbersSim.end();
        auto isNoSimEmergency =
            std::find(qeccnumResponse.eccNumbersNoSim.begin(), qeccnumResponse.eccNumbersNoSim.end(), request) !=
            qeccnumResponse.eccNumbersNoSim.end();

        if (isSimEmergency || isNoSimEmergency) {
            if (simInserted || isNoSimEmergency) {
                return std::make_unique<CallRequest>(request);
            }
            else {
                return std::make_unique<RejectRequest>(RejectRequest::RejectReason::NoSim, request);
            }
        }
        else if (callMode == cellular::api::CallMode::Emergency) {
            return std::make_unique<RejectRequest>(RejectRequest::RejectReason::NotAnEmergencyNumber, request);
        }
        return nullptr;
    }

    bool RequestFactory::isConnectedToNetwork()
    {
        at::Cmd buildCmd = at::factory(at::AT::COPS) + "?";
        auto resp        = channel.cmd(buildCmd);
        at::response::cops::CurrentOperatorInfo ret;
        if ((resp.code == at::Result::Code::OK) && (at::response::parseCOPS(resp, ret))) {
            return ret.getOperator().has_value();
        }
        return false;
    }

    std::unique_ptr<IRequest> RequestFactory::create()
    {
        if (auto req = emergencyCheck(); req) {
            return req;
        }

        std::string groupA, groupB, groupC, groupD, groupE, groupF;
        GroupMatch matchPack = {groupA, groupB, groupC, groupD, groupE, groupF};

        std::vector<std::string> results;

        for (const auto &element : requestMap) {
            auto const &requestCreateCallback = element.second;
            re2::StringPiece input(request);
            re2::RE2 reg(element.first);

            std::vector<RE2::Arg> reArguments;
            std::vector<RE2::Arg *> reArgumentPtrs;

            std::size_t numberOfGroups = reg.NumberOfCapturingGroups();

            if (numberOfGroups > matchPack.size()) {
                LOG_ERROR("Internal error, GroupMatch has to be redefined.");
                break;
            }

            reArguments.resize(numberOfGroups);
            reArgumentPtrs.resize(numberOfGroups);
            results.resize(numberOfGroups);

            for (std::size_t i = 0; i < numberOfGroups; ++i) {
                /// Bind RE arguments to strings from vector.
                reArguments[i] = &matchPack[i].get();
                /// Assign pointers to arguments.
                reArgumentPtrs[i] = &reArguments[i];
            }

            if (re2::RE2::FullMatchN(input, reg, reArgumentPtrs.data(), numberOfGroups)) {
                try {
                    if (auto req = requestCreateCallback(request, matchPack)) {
                        return req;
                    }
                }
                catch (const std::runtime_error &except) {
                    LOG_ERROR("Failed to create MMI request. Error message:\n%s", except.what());
                }
            }
        }

        if (!simInserted) {
            return std::make_unique<RejectRequest>(RejectRequest::RejectReason::NoSim, request);
        }
        auto isRegisteredToNetwork = isConnectedToNetwork();
        if (!isRegisteredToNetwork) {
            return std::make_unique<RejectRequest>(RejectRequest::RejectReason::NoNetworkConnection, request);
        }
        return std::make_unique<CallRequest>(request);
    }

} // namespace cellular
