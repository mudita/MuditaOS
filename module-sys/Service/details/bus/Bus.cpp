// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Bus.hpp"

#include "module-sys/Service/Service.hpp"
#include "module-os/CriticalSectionGuard.hpp"

#include "ticks.hpp"

#include <algorithm>
#include <cassert>
#include <set>

namespace sys
{
    namespace
    {
        std::uint64_t uniqueMsgId  = 0;
        std::uint64_t unicastMsgId = 0;

        std::map<BusChannel, std::set<Service *>> channels;
        std::map<std::string, Service *> servicesRegistered;
    } // namespace

    void Bus::Add(Service *service)
    {
        cpp_freertos::CriticalSectionGuard guard;

        for (auto channel : service->bus.channels) {
            channels[channel].insert(service);
        }
        servicesRegistered[service->GetName()] = service;
    }

    void Bus::Remove(Service *service)
    {
        cpp_freertos::CriticalSectionGuard guard;

        for (auto channel : service->bus.channels) {
            auto &services = channels[channel];
            services.erase(service);
        }
        servicesRegistered.erase(service->GetName());
    }

    void Bus::SendResponse(std::shared_ptr<Message> response, std::shared_ptr<Message> request, Service *sender)
    {
        assert(response != nullptr);
        assert(request != nullptr);
        assert(sender != nullptr);

        {
            cpp_freertos::CriticalSectionGuard guard;
            response->id    = uniqueMsgId++;
            response->uniID = request->uniID;
        }
        response->sender    = sender->GetName();
        response->transType = Message::TransmissionType::Unicast;
        if (auto it = servicesRegistered.find(request->sender); it != servicesRegistered.end()) {
            const auto targetService = it->second;
            targetService->mailbox.push(response);
        }
    }

    bool Bus::SendUnicast(std::shared_ptr<Message> message, const std::string &targetName, Service *sender)
    {
        {
            cpp_freertos::CriticalSectionGuard guard;
            message->id    = uniqueMsgId++;
            message->uniID = unicastMsgId++;
        }
        message->sender    = sender->GetName();
        message->transType = Message::TransmissionType::Unicast;

        if (auto it = servicesRegistered.find(targetName); it != servicesRegistered.end()) {
            const auto targetService = it->second;
            targetService->mailbox.push(message);
            return true;
        }

        LOG_ERROR("Service %s doesn't exist", targetName.c_str());
        return false;
    }

    SendResult Bus::SendUnicast(std::shared_ptr<Message> message,
                                const std::string &targetName,
                                Service *sender,
                                std::uint32_t timeout)
    {
        std::vector<std::shared_ptr<Message>> tempMsg;
        tempMsg.reserve(4); // reserve space for 4 elements to avoid costly memory allocations

        uint64_t unicastID = unicastMsgId;
        {
            cpp_freertos::CriticalSectionGuard guard;
            message->id    = uniqueMsgId++;
            message->uniID = unicastMsgId++;
        }
        message->sender    = sender->GetName();
        message->transType = Message::TransmissionType ::Unicast;

        auto ele = servicesRegistered.find(targetName);
        if (ele != servicesRegistered.end()) {
            const auto targetService = servicesRegistered[targetName];
            targetService->mailbox.push(message);
        }
        else {
            LOG_ERROR("Service %s doesn't exist", targetName.c_str());
            return std::make_pair(ReturnCodes::ServiceDoesntExist, nullptr);
        }

        uint32_t currentTime   = cpp_freertos::Ticks::GetTicks();
        uint32_t timeoutNeeded = currentTime + timeout;
        uint32_t timeElapsed   = currentTime;

        while (1) {
            // timeout
            if (timeElapsed >= timeoutNeeded) {

                // Push messages collected during waiting for response to processing queue
                for (const auto &w : tempMsg) {
                    sender->mailbox.push(w);
                }

                // Register that we didn't receive response. Even if it arrives it will be dropped
                sender->staleUniqueMsg.push_back(std::make_pair(unicastID, cpp_freertos::Ticks::GetTicks()));
                return std::make_pair(ReturnCodes::Timeout, nullptr);
            }

            // Immediately block on rx queue
            auto rxmsg = sender->mailbox.pop(timeoutNeeded - timeElapsed);

            timeElapsed = cpp_freertos::Ticks::GetTicks();

            // check for timeout
            if (rxmsg == nullptr) {

                // Push messages collected during waiting for response to processing queue
                for (const auto &w : tempMsg) {
                    sender->mailbox.push(w);
                }

                // Register that we didn't receive response. Even if it arrives it will be dropped
                sender->staleUniqueMsg.push_back(std::make_pair(unicastID, cpp_freertos::Ticks::GetTicks()));
                return CreateSendResult(ReturnCodes::Timeout, nullptr);
            }

            // Received response
            if ((rxmsg->uniID == unicastID) && (message->sender == sender->GetName())) {

                // Push messages collected during waiting for response to processing queue
                for (const auto &w : tempMsg) {
                    sender->mailbox.push(w);
                }

                return CreateSendResult(ReturnCodes::Success, rxmsg);
            }
            // Check for system messages
            else if (rxmsg->type == Message::Type::System) {

                SystemMessage *sysmsg = static_cast<SystemMessage *>(rxmsg.get());

                if (sysmsg->systemMessageType == SystemMessageType::Ping) {
                    rxmsg->Execute(sender);
                }
                else {
                    tempMsg.push_back(rxmsg);
                }
            }
            // Plain data message, save it for later
            else {
                tempMsg.push_back(rxmsg);
            }
        }
    }

    void Bus::SendMulticast(std::shared_ptr<Message> message, BusChannel channel, Service *sender)
    {
        {
            cpp_freertos::CriticalSectionGuard guard;
            message->id = uniqueMsgId++;
        }

        message->channel   = channel;
        message->transType = Message::TransmissionType::Multicast;
        message->sender    = sender->GetName();
        for (const auto &target : channels[channel]) {
            target->mailbox.push(message);
        }
    }

    void Bus::SendBroadcast(std::shared_ptr<Message> message, Service *sender)
    {
        {
            cpp_freertos::CriticalSectionGuard guard;
            message->id = uniqueMsgId++;
        }

        message->transType = Message::TransmissionType ::Broadcast;
        message->sender    = sender->GetName();
        for (const auto &target : servicesRegistered) {
            const auto targetService = target.second;
            targetService->mailbox.push(message);
        }
    }
} // namespace sys
