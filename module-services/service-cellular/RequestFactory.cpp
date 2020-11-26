// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
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

namespace cellular
{
    RequestFactory::RequestFactory(const std::string &data) : request(data)
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

    std::unique_ptr<IRequest> RequestFactory::create()
    {
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
        return std::make_unique<CallRequest>(request);
    }

} // namespace cellular
