// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceCellularPriv.hpp"
#include "messages.hpp"

#include <service-cellular-api>

#include <service-evtmgr/EVMessages.hpp>
#include <service-evtmgr/Constants.hpp>

#include <service-time/service-time/TimeMessage.hpp>
#include <service-time/Constants.hpp>
#include <queries/messages/sms/QuerySMSUpdate.hpp>

#include <at/ATFactory.hpp>
#include <ucs2/UCS2.hpp>
#include <service-appmgr/model/ApplicationManager.hpp>

#include "service-cellular/MessageConstants.hpp"
#include "checkSmsCenter.hpp"

using service::name::service_time;

namespace cellular::internal
{
    ServiceCellularPriv::ServiceCellularPriv(ServiceCellular *owner)
        : owner{owner}, simCard{std::make_unique<SimCard>()}, state{std::make_unique<State>(owner)},
          networkTime{std::make_unique<NetworkTime>()}, simContacts{std::make_unique<SimContacts>()}
    {
        initSimCard();
        initSMSSendHandler();
    }

    void ServiceCellularPriv::initSimCard()
    {
        using namespace cellular::msg;
        simCard->onSimReady = [this]() {
            state->set(State::ST::Ready);
            owner->bus.sendMulticast<notification::SimReady>();
        };
        simCard->onNeedPin = [this](unsigned int attempts) {
            owner->bus.sendMulticast<notification::SimNeedPin>(attempts);
        };
        simCard->onNeedPuk = [this](unsigned int attempts) {
            owner->bus.sendMulticast<notification::SimNeedPuk>(attempts);
        };
        simCard->onSimBlocked   = [this]() { owner->bus.sendMulticast<notification::SimBlocked>(); };
        simCard->onSimEvent     = [this]() { owner->bus.sendMulticast<notification::SimStateUpdate>(); };
        simCard->onUnhandledCME = [this](unsigned int code) {
            owner->bus.sendMulticast<notification::UnhandledCME>(code);
        };
    }

    void ServiceCellularPriv::connectSimCard()
    {
        using namespace cellular::msg;
        /**
         * Request message handlers
         */
        owner->connect(typeid(request::sim::SetActiveSim), [&](sys::Message *request) -> sys::MessagePointer {
            auto msg = static_cast<request::sim::SetActiveSim *>(request);
            return std::make_shared<request::sim::SetActiveSim::Response>(simCard->handleSetActiveSim(msg->sim));
        });
        owner->connect(typeid(request::sim::GetLockState), [&](sys::Message *) -> sys::MessagePointer {
            return std::make_shared<request::sim::GetLockState::Response>(simCard->handleIsPinLocked());
        });
        owner->connect(typeid(request::sim::ChangePin), [&](sys::Message *request) -> sys::MessagePointer {
            auto msg = static_cast<request::sim::ChangePin *>(request);
            return std::make_shared<request::sim::ChangePin::Response>(simCard->handleChangePin(msg->oldPin, msg->pin));
        });
        owner->connect(typeid(request::sim::UnblockWithPuk), [&](sys::Message *request) -> sys::MessagePointer {
            auto msg = static_cast<request::sim::UnblockWithPuk *>(request);
            return std::make_shared<request::sim::UnblockWithPuk::Response>(
                simCard->handleUnblockWithPuk(msg->puk, msg->pin));
        });
        owner->connect(typeid(request::sim::SetPinLock), [&](sys::Message *request) -> sys::MessagePointer {
            auto msg = static_cast<request::sim::SetPinLock *>(request);
            return std::make_shared<request::sim::SetPinLock::Response>(simCard->handleSetPinLock(msg->pin, msg->lock),
                                                                        msg->lock);
        });
        owner->connect(typeid(request::sim::PinUnlock), [&](sys::Message *request) -> sys::MessagePointer {
            auto msg = static_cast<request::sim::PinUnlock *>(request);
            return std::make_shared<request::sim::PinUnlock::Response>(simCard->handlePinUnlock(msg->pin));
        });

        /**
         * Notification message handlers
         */
        owner->connect(typeid(sevm::SIMTrayMessage), [&](sys::Message *request) -> sys::MessagePointer {
            simCard->handleTrayState();
            return sys::MessageNone{};
        });

        /**
         * Internal message handlers
         */
        owner->connect(typeid(internal::msg::HandleATSimStateChange),
                       [&](sys::Message *request) -> sys::MessagePointer {
                           auto msg = static_cast<internal::msg::HandleATSimStateChange *>(request);
                           simCard->handleATSimStateChange(msg->state);
                           return sys::MessageNone{};
                       });
    }

    void ServiceCellularPriv::connectNetworkTime()
    {
        owner->connect(typeid(stm::message::AutomaticDateAndTimeChangedMessage),
                       [&](sys::Message *request) -> sys::MessagePointer {
                           auto message = static_cast<stm::message::AutomaticDateAndTimeChangedMessage *>(request);
                           networkTime->processSettings(message->getValue());
                           return sys::MessageNone{};
                       });
        owner->connect(typeid(stm::message::GetAutomaticDateAndTimeResponse),
                       [&](sys::Message *request) -> sys::MessagePointer {
                           auto message = static_cast<stm::message::GetAutomaticDateAndTimeResponse *>(request);
                           networkTime->processSettings(message->isAutomaticDateAndTime());
                           return sys::MessageNone{};
                       });
    }

    void ServiceCellularPriv::requestNetworkTimeSettings()
    {
        owner->bus.sendUnicast(networkTime->createSettingsRequest(), service_time);
    }
    void ServiceCellularPriv::initSMSSendHandler()
    {
        outSMSHandler.onSendQuery = [this](db::Interface::Name database, std::unique_ptr<db::Query> query) -> bool {
            return DBServiceAPI::GetQuery(owner, database, std::move(query)).first;
        };

        outSMSHandler.onSend = [this](SMSRecord &record) -> bool { return this->onSendSMS(record); };

        outSMSHandler.onGetOfflineMode = [this]() -> bool {
            bool ret = false;
            if (owner->phoneModeObserver->isInMode(sys::phone_modes::PhoneMode::Offline)) {
                owner->bus.sendUnicast(std::make_shared<CellularSMSRejectedByOfflineNotification>(),
                                       app::manager::ApplicationManager::ServiceName);
                ret = true;
            }
            return ret;
        };
    }

    bool ServiceCellularPriv::onSendSMS(SMSRecord &record)
    {
        uint32_t textLen = record.body.length();

        auto commandTimeout                 = at::factory(at::AT::CMGS).getTimeout();
        constexpr uint32_t singleMessageLen = msgConstants::singleMessageMaxLen;
        bool result                         = false;
        auto channel                        = owner->cmux->get(CellularMux::Channel::Commands);
        auto receiver                       = record.number.getEntered();
        if (channel) {
            if (!channel->cmd(at::AT::SET_SMS_TEXT_MODE_UCS2)) {
                LOG_ERROR("Could not set UCS2 mode for SMS");
                return false;
            }
            if (!channel->cmd(at::AT::SMS_UCSC2)) {
                LOG_ERROR("Could not set UCS2 charset mode for TE");
                return false;
            }
            // if text fit in single message send
            if (textLen < singleMessageLen) {
                std::string command      = std::string(at::factory(at::AT::CMGS));
                std::string body         = UCS2(UTF8(receiver)).str();
                std::string suffix       = "\"";
                std::string command_data = command + body + suffix;
                if (owner->cmux->checkATCommandPrompt(
                        channel->sendCommandPrompt(command_data.c_str(), 1, commandTimeout))) {

                    if (channel->cmd((UCS2(record.body).str() + "\032").c_str(), commandTimeout)) {
                        result = true;
                    }
                    else {
                        result = false;
                    }
                    if (!result) {
                        LOG_ERROR("Message send failure");
                    }
                }
            }
            // split text, and send concatenated messages
            else {
                const uint32_t maxConcatenatedCount = msgConstants::maxConcatenatedCount;
                uint32_t messagePartsCount          = textLen / singleMessageLen;
                if ((textLen % singleMessageLen) != 0) {
                    messagePartsCount++;
                }

                if (messagePartsCount > maxConcatenatedCount) {
                    LOG_ERROR("Message to long");
                    result = false;
                }
                else {
                    auto channel = owner->cmux->get(CellularMux::Channel::Commands);

                    for (uint32_t i = 0; i < messagePartsCount; i++) {

                        uint32_t partLength = singleMessageLen;
                        if (i * singleMessageLen + singleMessageLen > record.body.length()) {
                            partLength = record.body.length() - i * singleMessageLen;
                        }
                        UTF8 messagePart = record.body.substr(i * singleMessageLen, partLength);

                        std::string command(at::factory(at::AT::QCMGS) + UCS2(UTF8(receiver)).str() + "\",120," +
                                            std::to_string(i + 1) + "," + std::to_string(messagePartsCount));

                        if (owner->cmux->checkATCommandPrompt(
                                channel->sendCommandPrompt(command.c_str(), 1, commandTimeout))) {
                            // prompt sign received, send data ended by "Ctrl+Z"
                            if (channel->cmd(UCS2(messagePart).str() + "\032", commandTimeout, 2)) {
                                result = true;
                            }
                            else {
                                result = false;
                                LOG_ERROR("Message send failure");
                                break;
                            }
                        }
                        else {
                            result = false;
                            LOG_ERROR("Message send failure");
                            break;
                        }
                    }
                }
            }
        }
        if (result) {
            LOG_INFO("SMS sent ok.");
            record.type = SMSType::OUTBOX;
        }
        else {
            LOG_ERROR("SMS sending failed.");
            record.type = SMSType::FAILED;
            if (!checkSmsCenter(*channel)) {
                LOG_ERROR("SMS center check failed");
            }
        }

        DBServiceAPI::GetQuery(
            owner, db::Interface::Name::SMS, std::make_unique<db::query::SMSUpdate>(std::move(record)));

        if (!channel->cmd(at::AT::SMS_GSM)) {
            LOG_ERROR("Could not set GSM (default) charset mode for TE");
            return false;
        }
        return result;
    }

    void ServiceCellularPriv::connectSimContacts()
    {
        owner->connect(typeid(cellular::GetSimContactsRequest), [&](sys::Message *request) -> sys::MessagePointer {
            std::vector<cellular::SimContact> contacts;
            if (simContacts->getContacts(contacts)) {
                return std::make_shared<cellular::GetSimContactsResponse>(
                    std::make_shared<std::vector<cellular::SimContact>>(contacts));
            }
            return std::make_shared<cellular::GetSimContactsResponse>();
        });
    }

} // namespace cellular::internal
