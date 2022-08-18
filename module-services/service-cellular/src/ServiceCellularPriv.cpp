// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceCellularPriv.hpp"
#include "messages.hpp"
#include "SMSPartsHandler.hpp"

#include <service-cellular-api>
#include <service-cellular/Constans.hpp>

#include <service-evtmgr/EVMessages.hpp>
#include <service-evtmgr/Constants.hpp>

#include <service-bluetooth/messages/BluetoothModeChanged.hpp>
#include <locks/data/PhoneLockMessages.hpp>
#include <service-time/service-time/TimeMessage.hpp>
#include <service-time/Constants.hpp>
#include <queries/messages/sms/QuerySMSUpdate.hpp>

#include <service-antenna/AntennaServiceAPI.hpp>

#include <at/ATFactory.hpp>
#include <at/cmd/QCFGUsbnet.hpp>
#include <at/cmd/CSQ.hpp>
#include <ucs2/UCS2.hpp>
#include <service-appmgr/Constants.hpp>

#include "service-cellular/MessageConstants.hpp"
#include "checkSmsCenter.hpp"

using service::name::service_time;

namespace cellular::internal
{
    ServiceCellularPriv::ServiceCellularPriv(ServiceCellular *owner)
        : owner{owner}, simCard{std::make_unique<SimCard>()}, state{std::make_unique<State>(owner)},
          networkTime{std::make_unique<NetworkTime>()}, simContacts{std::make_unique<SimContacts>()},
          imeiGetHandler{std::make_unique<service::ImeiGetHandler>()},
          tetheringHandler{std::make_unique<TetheringHandler>()},
          modemResetHandler{std::make_unique<ModemResetHandler>()}, csqHandler{std::make_unique<CSQHandler>()}
    {
        initSimCard();
        initSMSSendHandler();
        initTetheringHandler();
        initModemResetHandler();
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
        simCard->onSimNotPresent = [this]() { owner->bus.sendMulticast<notification::SimNotInserted>(); };
        simCard->onSimSelected   = [this]() {
            owner->connectionManager->onPhoneModeChange(owner->phoneModeObserver->getCurrentPhoneMode());
            requestNetworkTimeSettings();
        };
    }

    void ServiceCellularPriv::connectSimCard()
    {
        using namespace cellular::msg;
        /**
         * Request message handlers
         */
        owner->connect(typeid(request::sim::SetActiveSim), [&](sys::Message *request) -> sys::MessagePointer {
            auto msg    = static_cast<request::sim::SetActiveSim *>(request);
            auto result = simCard->handleSetActiveSim(msg->sim);
            owner->simTimer.start();
            simCard->handleSimCardSelected();
            return std::make_shared<request::sim::SetActiveSim::Response>(result);
        });
        owner->connect(typeid(request::sim::GetLockState), [&](sys::Message *) -> sys::MessagePointer {
            if (!simCard->isSimCardInserted()) {
                owner->bus.sendMulticast<notification::SimNotInserted>();
                return sys::MessageNone{};
            }
            return std::make_shared<request::sim::GetLockState::Response>(simCard->handleIsPinLocked());
        });
        owner->connect(typeid(request::sim::ChangePin), [&](sys::Message *request) -> sys::MessagePointer {
            auto msg = static_cast<request::sim::ChangePin *>(request);
            if (!simCard->isSimCardInserted()) {
                owner->bus.sendMulticast<notification::SimNotInserted>();
                return sys::MessageNone{};
            }
            return std::make_shared<request::sim::ChangePin::Response>(simCard->handleChangePin(msg->oldPin, msg->pin));
        });
        owner->connect(typeid(request::sim::UnblockWithPuk), [&](sys::Message *request) -> sys::MessagePointer {
            auto msg = static_cast<request::sim::UnblockWithPuk *>(request);
            if (!simCard->isSimCardInserted()) {
                owner->bus.sendMulticast<notification::SimNotInserted>();
                return sys::MessageNone{};
            }
            return std::make_shared<request::sim::UnblockWithPuk::Response>(
                simCard->handleUnblockWithPuk(msg->puk, msg->pin));
        });
        owner->connect(typeid(request::sim::SetPinLock), [&](sys::Message *request) -> sys::MessagePointer {
            auto msg = static_cast<request::sim::SetPinLock *>(request);
            if (!simCard->isSimCardInserted()) {
                owner->bus.sendMulticast<notification::SimNotInserted>();
                return sys::MessageNone{};
            }
            return std::make_shared<request::sim::SetPinLock::Response>(simCard->handleSetPinLock(msg->pin, msg->lock),
                                                                        msg->lock);
        });
        owner->connect(typeid(request::sim::PinUnlock), [&](sys::Message *request) -> sys::MessagePointer {
            auto msg = static_cast<request::sim::PinUnlock *>(request);
            if (!simCard->isSimCardInserted()) {
                owner->bus.sendMulticast<notification::SimNotInserted>();
                return sys::MessageNone{};
            }
            return std::make_shared<request::sim::PinUnlock::Response>(simCard->handlePinUnlock(msg->pin));
        });

        /**
         * Notification message handlers
         */
        owner->connect(typeid(sevm::SIMTrayMessage), [&](sys::Message *request) -> sys::MessagePointer {
            simCard->handleTrayState();
            return sys::MessageNone{};
        });
        owner->connect(typeid(cellular::SimInsertedNotication), [&](sys::Message *request) -> sys::MessagePointer {
            auto message = static_cast<cellular::SimInsertedNotication *>(request);

            if (simCard->isSimSelectInProgress()) {
                return sys::MessageNone{};
            }
            simCard->handleSimInsertionNotification(message->getInsertedStatus());
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

                           networkTime->processSettings(
                               message->getValue(),
                               owner->phoneModeObserver->isInMode(sys::phone_modes::PhoneMode::Offline));
                           return sys::MessageNone{};
                       });
        owner->connect(
            typeid(stm::message::GetAutomaticDateAndTimeResponse), [&](sys::Message *request) -> sys::MessagePointer {
                auto message = static_cast<stm::message::GetAutomaticDateAndTimeResponse *>(request);
                networkTime->processSettings(message->isAutomaticDateAndTime(),
                                             owner->phoneModeObserver->isInMode(sys::phone_modes::PhoneMode::Offline));
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

        outSMSHandler.onSIMNotInitialized = [this]() -> bool {
            bool ret = false;
            if (!simCard->initialized()) {
                owner->bus.sendUnicast(std::make_shared<cellular::SmsNoSimRequestMessage>(), ::service::name::appmgr);
                ret = true;
            }
            return ret;
        };

        outSMSHandler.onGetOfflineMode = [this]() -> bool {
            bool ret = false;
            if (owner->phoneModeObserver->isInMode(sys::phone_modes::PhoneMode::Offline)) {
                owner->bus.sendUnicast(std::make_shared<cellular::SMSRejectedByOfflineNotification>(),
                                       ::service::name::appmgr);
                ret = true;
            }
            return ret;
        };
    }

    bool ServiceCellularPriv::onSendSMS(SMSRecord &record)
    {
        auto commandTimeout = at::factory(at::AT::CMGS).getTimeout();
        bool result         = false;
        auto channel        = owner->cmux->get(CellularMux::Channel::Commands);
        auto receiver       = record.number.getEntered();
        bool channelSetup   = false;

        if (channel) {
            channelSetup = true;
            if (!channel->cmd(at::AT::SET_SMS_TEXT_MODE_UCS2)) {
                LOG_ERROR("Could not set UCS2 mode for SMS");
                channelSetup = false;
            }
            if (!channel->cmd(at::AT::SMS_UCSC2)) {
                LOG_ERROR("Could not set UCS2 charset mode for TE");
                channelSetup = false;
            }
        }

        if (channelSetup) {
            auto partHandler = sms::SMSPartsHandler(record.body);
            if (partHandler.isSinglePartSMS()) {
                std::string command      = std::string(at::factory(at::AT::CMGS));
                std::string body         = UCS2(UTF8(receiver)).str();
                std::string suffix       = "\"";
                std::string command_data = command + body + suffix;
                if (owner->cmux->checkATCommandPrompt(
                        channel->sendCommandPrompt(command_data.c_str(), 1, commandTimeout))) {

                    if (channel->cmd((partHandler.getNextSmsPart() + "\032").c_str(), commandTimeout)) {
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
            else {
                if (partHandler.isPartsCountAboveLimit()) {
                    LOG_ERROR("Message too long.");
                    result = false;
                }
                else {
                    auto channel = owner->cmux->get(CellularMux::Channel::Commands);
                    saveNewMultiPartSMSUIDCallback(multiPartSMSUID);

                    for (unsigned i = 0; i < partHandler.getPartsCount(); i++) {

                        std::string command(at::factory(at::AT::QCMGS) + UCS2(UTF8(receiver)).str() + "\"," +
                                            std::to_string(multiPartSMSUID) + "," + std::to_string(i + 1) + "," +
                                            std::to_string(partHandler.getPartsCount()));

                        if (owner->cmux->checkATCommandPrompt(
                                channel->sendCommandPrompt(command.c_str(), 1, commandTimeout))) {
                            // prompt sign received, send data ended by "Ctrl+Z"

                            if (channel->cmd(partHandler.getNextSmsPart() + "\032", commandTimeout, 2)) {
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
                    multiPartSMSUID++;
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

        if (channel && !channel->cmd(at::AT::SMS_GSM)) {
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

    void ServiceCellularPriv::setInitialMultiPartSMSUID(std::uint8_t uid)
    {
        multiPartSMSUID = uid + 1;
    }

    void ServiceCellularPriv::connectImeiGetHandler()
    {
        owner->connect(typeid(cellular::GetImeiRequest), [&](sys::Message *request) -> sys::MessagePointer {
            std::string imei;
            if (imeiGetHandler->getImei(imei)) {
                return std::make_shared<cellular::GetImeiResponse>(std::make_shared<std::string>(imei));
            }
            return std::make_shared<cellular::GetImeiResponse>();
        });
    }
    void ServiceCellularPriv::initTetheringHandler()
    {
        tetheringHandler->onIsRndisEnabled = [this]() -> std::optional<at::qcfg::usbnet::Net> {
            auto channel = owner->cmux->get(CellularMux::Channel::Commands);
            if (channel == nullptr) {
                return std::nullopt;
            }
            auto command  = at::cmd::QCFGUsbnet(at::cmd::Modifier::None);
            auto response = channel->cmd(command);
            auto result   = command.parseQCFGUsbnet(response);
            if (result) {
                return result.net;
            }
            return std::nullopt;
        };

        tetheringHandler->onEnableRndis = [this]() -> bool {
            auto channel = owner->cmux->get(CellularMux::Channel::Commands);
            if (channel == nullptr) {
                return false;
            }
            auto response = channel->cmd(at::AT::SET_RNDIS);
            if (response) {
                return true;
            }
            return false;
        };

        tetheringHandler->onEnableUrc = [this]() -> bool { return owner->tetheringTurnOnURC(); };

        tetheringHandler->onDisableUrc = [this]() -> bool { return owner->tetheringTurnOffURC(); };

        tetheringHandler->onSetPassthrough = [](bool enable) {
            using bsp::cellular::USB::setPassthrough;
            using bsp::cellular::USB::PassthroughState;
            setPassthrough(enable ? PassthroughState::ENABLED : PassthroughState::DISABLED);
        };

        tetheringHandler->onReadMessages = [this]() -> bool { return owner->receiveAllMessages(); };
    }

    void ServiceCellularPriv::initModemResetHandler()
    {
        modemResetHandler->onCellularStateSet = [this](cellular::service::State::ST newState) { state->set(newState); };

        modemResetHandler->onTurnModemOn = [this]() { owner->cmux->turnOnModem(); };

        modemResetHandler->onTurnModemOff = [this]() { owner->cmux->turnOffModem(); };

        modemResetHandler->onHardReset = [this]() { owner->cmux->resetModem(); };

        modemResetHandler->onSoftReset = [this]() -> bool {
            auto channel = owner->cmux->get(CellularMux::Channel::Commands);
            if (channel == nullptr) {
                return false;
            }
            auto response = channel->cmd(at::AT::CFUN_RESET);
            if (response) {
                return true;
            }
            return false;
        };
    }

    void ServiceCellularPriv::initCSQHandler()
    {
        csqHandler->onEnableCsqReporting = [this]() {
            if (owner->cpuSentinel) {
                owner->cpuSentinel->HoldMinimumFrequency(bsp::CpuFrequencyMHz::Level_2);
            }

            auto channel = owner->cmux->get(CellularMux::Channel::Commands);
            if (channel) {
                auto result = channel->cmd(at::AT::CSQ_URC_ON);
                if (result) {
                    owner->csqCounter.clearCounter();
                    return true;
                }
            }
            return false;
        };

        csqHandler->onDisableCsqReporting = [this]() {
            if (owner->cpuSentinel) {
                owner->cpuSentinel->HoldMinimumFrequency(bsp::CpuFrequencyMHz::Level_2);
            }

            auto channel = owner->cmux->get(CellularMux::Channel::Commands);
            if (channel) {
                auto result = channel->cmd(at::AT::CSQ_URC_OFF);
                if (result) {
                    return true;
                }
            }
            return false;
        };

        csqHandler->onGetCsq = [this]() -> std::optional<at::result::CSQ> {
            if (owner->cpuSentinel) {
                owner->cpuSentinel->HoldMinimumFrequency(bsp::CpuFrequencyMHz::Level_2);
            }

            auto channel = owner->cmux->get(CellularMux::Channel::Commands);
            if (!channel) {
                return std::nullopt;
            }
            auto command  = at::cmd::CSQ();
            auto response = channel->cmd(command);
            if (response.code == at::Result::Code::OK) {
                auto result = command.parseCSQ(response);
                if (result) {
                    return result;
                }
            }
            return std::nullopt;
        };

        csqHandler->onPropagateCSQ = [this](uint32_t csq) {
            SignalStrength signalStrength(static_cast<int>(csq));
            Store::GSM::get()->setSignalStrength(signalStrength.data);
            auto message = std::make_shared<cellular::SignalStrengthUpdateNotification>("");
            owner->bus.sendMulticast(message, sys::BusChannel::ServiceCellularNotifications);
        };

        csqHandler->onInvalidCSQ = [this]() { AntennaServiceAPI::InvalidCSQNotification(owner); };

        csqHandler->onRetrySwitchMode = [this](service::CSQMode newMode) {
            switch (newMode) {
            case service::CSQMode::PermanentReporting:
                owner->bus.sendUnicast(
                    std::make_shared<RetrySwitchCSQMode>(RetrySwitchCSQMode::Mode::PermanentReporting),
                    ::service::name::cellular);
                break;
            case service::CSQMode::HybridReporting:
                owner->bus.sendUnicast(std::make_shared<RetrySwitchCSQMode>(RetrySwitchCSQMode::Mode::HybridReporting),
                                       ::service::name::cellular);
                break;
            case service::CSQMode::HybridPolling:
                owner->bus.sendUnicast(std::make_shared<RetrySwitchCSQMode>(RetrySwitchCSQMode::Mode::HybridPolling),
                                       ::service::name::cellular);
                break;
            }
        };

        csqHandler->onRetryGetCSQ = [this]() {
            owner->bus.sendUnicast(std::make_shared<RetryGetCSQ>(), ::service::name::cellular);
        };

        owner->csqTimer.start();
    }

    void ServiceCellularPriv::connectCSQHandler()
    {
        owner->bus.channels.push_back(sys::BusChannel::PhoneLockChanges);
        owner->bus.channels.push_back(sys::BusChannel::BluetoothModeChanges);

        owner->connect(typeid(URCCounterMessage), [&](sys::Message *request) -> sys::MessagePointer {
            auto message = dynamic_cast<cellular::URCCounterMessage *>(request);
            csqHandler->handleURCCounterMessage(message->getCounter());
            return sys::MessageNone{};
        });

        owner->connect(typeid(RetrySwitchCSQMode), [&](sys::Message *request) -> sys::MessagePointer {
            auto message = dynamic_cast<cellular::RetrySwitchCSQMode *>(request);
            switch (message->getNewMode()) {
            case cellular::RetrySwitchCSQMode::Mode::PermanentReporting:
                csqHandler->switchToPermanentReportMode();
                break;
            case cellular::RetrySwitchCSQMode::Mode::HybridReporting:
                csqHandler->switchToHybridReportMode();
                break;
            case cellular::RetrySwitchCSQMode::Mode::HybridPolling:
                csqHandler->switchToHybridPollMode();
                break;
            }
            return sys::MessageNone{};
        });

        owner->connect(typeid(RetryGetCSQ), [&](sys::Message *request) -> sys::MessagePointer {
            csqHandler->getCSQ();
            return sys::MessageNone{};
        });

        owner->connect(typeid(locks::UnlockedPhone), [&](sys::Message *request) -> sys::MessagePointer {
            csqHandler->handleUnlockPhone();
            csqHandler->checkConditionToChangeMode();
            return sys::MessageNone{};
        });

        owner->connect(typeid(locks::LockedPhone), [&](sys::Message *request) -> sys::MessagePointer {
            csqHandler->handleLockPhone();
            csqHandler->checkConditionToChangeMode();
            return sys::MessageNone{};
        });

        owner->connect(typeid(sys::bluetooth::BluetoothModeChanged), [&](sys::Message *request) -> sys::MessagePointer {
            auto data   = static_cast<sys::bluetooth::BluetoothModeChanged *>(request);
            auto btMode = data->getBluetoothMode();
            if (btMode == sys::bluetooth::BluetoothMode::ConnectedVoice ||
                btMode == sys::bluetooth::BluetoothMode::ConnectedBoth) {
                csqHandler->handleBluetoothCarKitConnect();
            }
            else {
                csqHandler->handleBluetoothCarKitDisconnect();
            }
            csqHandler->checkConditionToChangeMode();
            return sys::MessageNone{};
        });

        owner->connect(typeid(sevm::BatteryStatusChangeMessage), [&](sys::Message *request) -> sys::MessagePointer {
            csqHandler->checkConditionToChangeMode();
            return sys::MessageNone{};
        });
    }
} // namespace cellular::internal
