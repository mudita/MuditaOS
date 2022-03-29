// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Bus.hpp"

#include <Service/Service.hpp>
#include <log/log.hpp>
#include "SystemWatchdog/SystemWatchdog.hpp"
#include "module-os/CriticalSectionGuard.hpp"

#include "ticks.hpp"

#include <algorithm>
#include <cassert>
#include <set>

namespace sys
{
    namespace
    {
        MessageUID uniqueMsgId;
        MessageUID unicastMsgId;

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

        response->sender    = sender->GetName();
        response->transType = Message::TransmissionType::Unicast;

        if (request->transType == Message::TransmissionType::Unicast) {
            {
                cpp_freertos::CriticalSectionGuard guard;
                response->id    = uniqueMsgId.getNext();
                response->uniID = request->uniID;
            }

            response->ValidateUnicastMessage();
        }
        else {
            {
                cpp_freertos::CriticalSectionGuard guard;
                response->id = uniqueMsgId.getNext();
            }

            response->ValidateResponseMessage();
        }

        if (auto it = servicesRegistered.find(request->sender); it != servicesRegistered.end()) {
            const auto targetService = it->second;
            targetService->mailbox.push(response);
        }
    }

    bool Bus::SendUnicast(std::shared_ptr<Message> message, const std::string &targetName, Service *sender)
    {
        {
            cpp_freertos::CriticalSectionGuard guard;
            message->id    = uniqueMsgId.getNext();
            message->uniID = unicastMsgId.getNext();
        }

        message->sender    = sender->GetName();
        message->transType = Message::TransmissionType::Unicast;

        message->ValidateUnicastMessage();

        if (auto it = servicesRegistered.find(targetName); it != servicesRegistered.end()) {
            const auto targetService = it->second;
            targetService->mailbox.push(message);
            return true;
        }

        LOG_ERROR("Service %s doesn't exist", targetName.c_str());
        return false;
    }

    namespace
    {
        template <class W, class X> void restoreMessagess(W &mailbox, X &tempMsg)
        {
            // Push messages collected during waiting for response to processing queue
            for (const auto &w : tempMsg) {
                mailbox.push(w);
            }
        }
    } // namespace

    SendResult Bus::SendUnicastSync(std::shared_ptr<Message> message,
                                    const std::string &targetName,
                                    Service *sender,
                                    std::uint32_t timeout)
    {
        {
            cpp_freertos::CriticalSectionGuard guard;
            message->id    = uniqueMsgId.getNext();
            message->uniID = unicastMsgId.getNext();
        }

        message->sender    = sender->GetName();
        message->transType = Message::TransmissionType::Unicast;

        message->ValidateUnicastMessage();

        auto ele = servicesRegistered.find(targetName);
        if (ele != servicesRegistered.end()) {
            const auto targetService = servicesRegistered[targetName];
            targetService->mailbox.push(message);
        }
        else {
            LOG_ERROR("Service %s doesn't exist", targetName.c_str());
            return std::make_pair(ReturnCodes::ServiceDoesntExist, nullptr);
        }

        return UnicastSync(message, sender, timeout);
    }

    SendResult Bus::UnicastSync(const std::shared_ptr<Message> &message, Service *sender, std::uint32_t timeout)
    {
        std::vector<std::shared_ptr<Message>> tempMsg;
        tempMsg.reserve(4); // reserve space for 4 elements to avoid costly memory allocations

        // NOTE: please mind that timeout + curentTime might overflow 32b
        // especially if we want to wait infinitely
        uint64_t currentTime   = cpp_freertos::Ticks::GetTicks();
        uint64_t timeoutNeeded = currentTime + timeout;
        uint64_t timeElapsed   = currentTime;

        while (1) {
            // timeout
            if (timeElapsed >= timeoutNeeded) {
                restoreMessagess(sender->mailbox, tempMsg);
                return std::make_pair(ReturnCodes::Timeout, nullptr);
            }

            // Immediately block on rx queue
            auto rxmsg = sender->mailbox.pop(timeoutNeeded - timeElapsed);
            timeElapsed = cpp_freertos::Ticks::GetTicks();

            // check for timeout
            if (rxmsg == nullptr) {
                restoreMessagess(sender->mailbox, tempMsg);
                return CreateSendResult(ReturnCodes::Timeout, nullptr);
            }

            // Received response
            if ((rxmsg->uniID == message->uniID) && (message->sender == sender->GetName())) {
                restoreMessagess(sender->mailbox, tempMsg);
                return CreateSendResult(ReturnCodes::Success, rxmsg);
            }
            tempMsg.push_back(rxmsg);
        }
    }

    void Bus::SendMulticast(std::shared_ptr<Message> message, BusChannel channel, Service *sender)
    {
        {
            cpp_freertos::CriticalSectionGuard guard;
            message->id = uniqueMsgId.getNext();
        }

        message->channel   = channel;
        message->transType = Message::TransmissionType::Multicast;
        message->sender    = sender->GetName();

        message->ValidateMulticastMessage();

        for (const auto &target : channels[channel]) {
            target->mailbox.push(message);
        }
    }

    void Bus::SendBroadcast(std::shared_ptr<Message> message, Service *sender)
    {
        {
            cpp_freertos::CriticalSectionGuard guard;
            message->id = uniqueMsgId.getNext();
        }

        message->transType = Message::TransmissionType::Broadcast;
        message->sender    = sender->GetName();

        message->ValidateBroadcastMessage();

        for (const auto &target : servicesRegistered) {
            const auto targetService = target.second;
            targetService->mailbox.push(message);
        }
    }
} // namespace sys
