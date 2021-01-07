// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Bus.hpp"
#include "thread.hpp"
#include "ticks.hpp"
#include "critical.hpp"
#include "Service.hpp"
#include "Message.hpp"
#include <algorithm>
#include <cassert>

namespace sys
{

    using namespace cpp_freertos;
    using namespace std;

    Bus::Bus()
    {}

    Bus::~Bus()
    {}

    void Bus::Add(std::shared_ptr<Service> service)
    {
        CriticalSection::Enter();
        for (auto &w : service->busChannels) {
            channels[w].m_services.insert(service);
        }
        servicesRegistered[service->GetName()] = service;
        CriticalSection::Exit();
    }

    void Bus::Remove(std::shared_ptr<Service> service)
    {
        CriticalSection::Enter();
        for (auto &w : service->busChannels) {

            auto it = std::find_if(channels[w].m_services.begin(),
                                   channels[w].m_services.end(),
                                   [&](std::shared_ptr<Service> const &s) { return s == service; });
            if (it != channels[w].m_services.end()) {
                channels[w].m_services.erase(it);
            }
            else {
            }
        }

        auto ele = servicesRegistered.find(service->GetName());
        if (ele != servicesRegistered.end()) {
            servicesRegistered.erase(service->GetName());
        }
        else {
            // error!, trying to remove non-existing service
        }

        CriticalSection::Exit();
    }

    void Bus::SendResponse(std::shared_ptr<Message> response, std::shared_ptr<Message> request, Service *sender)
    {
        assert(response != nullptr);
        assert(request != nullptr);
        assert(sender != nullptr);

        response->id        = uniqueMsgId++;
        response->uniID     = request->uniID;
        response->sender    = sender->GetName();
        response->transType = Message::TransmissionType ::Unicast;
        if (servicesRegistered.find(request->sender) != servicesRegistered.end()) {
            servicesRegistered[request->sender]->mailbox.push(response);
        }
        else {
            // silently drop it
        }
    }

    bool Bus::SendUnicast(std::shared_ptr<Message> msg, const std::string &service, Service *s)
    {
        CriticalSection::Enter();
        msg->id    = uniqueMsgId++;
        msg->uniID = unicastMsgId++;
        CriticalSection::Exit();

        msg->sender    = s->GetName();
        msg->transType = Message::TransmissionType ::Unicast;

        auto ele = servicesRegistered.find(service);
        if (ele != servicesRegistered.end()) {
            servicesRegistered[service]->mailbox.push(msg);
        }
        else {
            LOG_ERROR("Service %s doesn't exist", service.c_str());
            return false;
        }

        return true;
    }

    SendResult Bus::SendUnicast(std::shared_ptr<Message> msg, const std::string &service, Service *s, uint32_t timeout)
    {
        std::vector<std::shared_ptr<Message>> tempMsg;
        tempMsg.reserve(4); // reserve space for 4 elements to avoid costly memory allocations

        CriticalSection::Enter();
        uint64_t unicastID = unicastMsgId;
        msg->id            = uniqueMsgId++;
        msg->uniID         = unicastMsgId++;
        CriticalSection::Exit();

        msg->sender    = s->GetName();
        msg->transType = Message::TransmissionType ::Unicast;

        auto ele = servicesRegistered.find(service);
        if (ele != servicesRegistered.end()) {
            servicesRegistered[service]->mailbox.push(msg);
        }
        else {
            LOG_ERROR("Service %s doesn't exist", service.c_str());
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
                    s->mailbox.push(w);
                }

                // Register that we didn't receive response. Even if it arrives it will be dropped
                s->staleUniqueMsg.push_back(std::make_pair(unicastID, cpp_freertos::Ticks::GetTicks()));
                return std::make_pair(ReturnCodes::Timeout, nullptr);
            }

            // Immediately block on rx queue
            auto rxmsg = s->mailbox.pop(timeoutNeeded - timeElapsed);

            timeElapsed = cpp_freertos::Ticks::GetTicks();

            // check for timeout
            if (rxmsg == nullptr) {

                // Push messages collected during waiting for response to processing queue
                for (const auto &w : tempMsg) {
                    s->mailbox.push(w);
                }

                // Register that we didn't receive response. Even if it arrives it will be dropped
                s->staleUniqueMsg.push_back(std::make_pair(unicastID, cpp_freertos::Ticks::GetTicks()));
                return CreateSendResult(ReturnCodes::Timeout, nullptr);
            }

            // Received response
            if ((rxmsg->uniID == unicastID) && (msg->sender == s->GetName())) {

                // Push messages collected during waiting for response to processing queue
                for (const auto &w : tempMsg) {
                    s->mailbox.push(w);
                }

                return CreateSendResult(ReturnCodes::Success, rxmsg);
            }
            // Check for system messages
            else if (rxmsg->type == Message::Type::System) {

                SystemMessage *sysmsg = static_cast<SystemMessage *>(rxmsg.get());

                if (sysmsg->systemMessageType == SystemMessageType::Ping) {
                    rxmsg->Execute(s);
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

    void Bus::SendMulticast(std::shared_ptr<Message> msg, BusChannels channel, Service *source)
    {
        CriticalSection::Enter();
        msg->id      = uniqueMsgId++;
        msg->channel = channel;
        CriticalSection::Exit();

        msg->transType = Message::TransmissionType ::Multicast;
        msg->sender    = source->GetName();

        for (auto const &w : channels[channel].m_services) {
            w->mailbox.push(msg);
        }
    }

    void Bus::SendBroadcast(std::shared_ptr<Message> msg, Service *source)
    {
        CriticalSection::Enter();
        msg->id = uniqueMsgId++;
        CriticalSection::Exit();

        msg->transType = Message::TransmissionType ::Broadcast;
        msg->sender    = source->GetName();

        for (auto const &w : servicesRegistered) {
            w.second->mailbox.push(msg);
        }
    }

    std::map<BusChannels, Channel> Bus::channels;
    std::map<std::string, std::shared_ptr<Service>> Bus::servicesRegistered;
    uint64_t Bus::uniqueMsgId;
    uint64_t Bus::unicastMsgId;

} // namespace sys
