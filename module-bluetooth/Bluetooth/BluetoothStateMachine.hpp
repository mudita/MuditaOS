// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#define BOOST_SML_CFG_DISABLE_MIN_SIZE // GCC10 fix
#include <boost/sml.hpp>
#include <stdexcept>
#include "command/event/Events.hpp"
#include "service-bluetooth/SettingsHolder.hpp"
#include "interface/profiles/ProfileManager.hpp"
#include "WorkerController.hpp"
#include "Device.hpp"
#include <log/log.hpp>
#include <sml-utils/Logger.hpp>

namespace bluetooth
{
    namespace sml = boost::sml;

    class InitializationError : public std::runtime_error
    {
      public:
        using std::runtime_error::runtime_error;
    };

    class ProcessingError : public std::runtime_error
    {
      public:
        using std::runtime_error::runtime_error;
    };

    struct InitializationState
    {
        bool isInitDone = false;
    };

    struct InitDriver
    {
        void operator()(std::shared_ptr<AbstractDriver> driver)
        {
            if (driver == nullptr) {
                throw std::runtime_error("shouldn't happen");
            }
            // printf("driver: 0x%X %d\n", driver.get(), int(driver.use_count()));
            if (const auto status = driver->init(); status != Result::Code::Success) {
                throw InitializationError{"Unable to initialize a bluetooth driver."};
            }
        }
    } constexpr InitDriver;

    struct InitDevicesList
    {
        void operator()(const std::shared_ptr<bluetooth::SettingsHolder> settings,
                        const std::shared_ptr<std::vector<Devicei>> pairedDevices)
        {
            auto bondedDevicesStr =
                std::visit(bluetooth::StringVisitor(), settings->getValue(bluetooth::Settings::BondedDevices));
            pairedDevices->clear();
            auto devices = SettingsSerializer::fromString(bondedDevicesStr);
            pairedDevices->assign(devices.begin(), devices.end());
            LOG_INFO("Loaded: %d devices", int(pairedDevices->size()));
        }
    } constexpr InitDevicesList;

    struct IsInit
    {
        bool operator()(InitializationState &data)
        {
            return data.isInitDone;
        };
    } constexpr isInit;

    struct PostInit
    {
        void operator()(DeviceRegistrationFunction registerDevice, InitializationState &data)
        {
            if (const auto status = registerDevice(); status != Result::Code::Success) {
                throw InitializationError{"Unable to initialize bluetooth"};
            }
            data.isInitDone = true;
        }
    } constexpr PostInit;

    struct StartDriver
    {
        void operator()(std::shared_ptr<AbstractDriver> driver)
        {
            if (const auto status = driver->run(); status != Result::Code::Success) {
                throw InitializationError{"Unable to run the bluetooth driver"};
            }
        }
    } static StartDriver;

    struct HandleOn
    {
        void operator()(std::shared_ptr<AbstractCommandHandler> handler)
        {
            handler->scan();
        }
    } constexpr HandleOn;

    struct HandleOff
    {
        void operator()(std::shared_ptr<AbstractCommandHandler> handler)
        {
            handler->stopScan();
        }
    } constexpr HandleOff;

    struct HandlePair
    {
        void operator()(std::shared_ptr<AbstractCommandHandler> handler, const bluetooth::event::Pair &event)
        {
            handler->pair(event.device);
        }
    } constexpr HandlePair;

    struct Connected
    {
        bool operator()(std::shared_ptr<bluetooth::SettingsHolder> settings, bluetooth::event::Unpair evt)
        {
            auto deviceAddr =
                std::visit(bluetooth::StringVisitor(), settings->getValue(bluetooth::Settings::ConnectedDevice));
            return static_cast<bool>(deviceAddr == bd_addr_to_str(evt.device.address));
        }
    } constexpr Connected;

    struct HandleUnpair
    {
        void operator()(std::shared_ptr<AbstractCommandHandler> handler, const bluetooth::event::Unpair &event)
        {
            handler->unpair(event.device);
        }
    } constexpr HandleUnpair;

    struct HandleDrop
    {
        void operator()(const bluetooth::event::Unpair &event,
                        std::shared_ptr<bluetooth::SettingsHolder> settings,
                        std::shared_ptr<std::vector<Devicei>> pairedDevices)
        {
            auto position = std::find_if(pairedDevices->begin(), pairedDevices->end(), [&](const Devicei &device) {
                return !bd_addr_cmp(event.device.address, device.address);
            });
            if (position != pairedDevices->end()) {
                pairedDevices->erase(position);
                settings->setValue(bluetooth::Settings::BondedDevices, SettingsSerializer::toString(*pairedDevices));
                LOG_INFO("Device removed from paired devices list");
            }
        }
    } constexpr HandleDrop;

    struct HandleConnect
    {
        void operator()(std::shared_ptr<AbstractCommandHandler> handler, bluetooth::event::Connect evt)
        {
            handler->connect(evt.device);
        }

    } constexpr HandleConnect;

    struct HandleDisconnect
    {
        void operator()(std::shared_ptr<AbstractCommandHandler> handler)
        {
            handler->disconnect();
        }

    } constexpr HandleDisconnect;

    struct HandleSetVisibility
    {
        void operator()(std::shared_ptr<AbstractCommandHandler> handler)
        {
            handler->setVisibility(true);
        }

    } constexpr HandleSetVisibility;

    struct HandleUnsetVisibility
    {
        void operator()(std::shared_ptr<AbstractCommandHandler> handler)
        {
            handler->setVisibility(false);
        }

    } constexpr HandleUnsetVisibility;

    struct IncomingCallStarted
    {
        void operator()(std::shared_ptr<bluetooth::BaseProfileManager> profileManager)
        {
            profileManager->incomingCallStarted();
        }
    } constexpr IncomingCallStarted;

    struct IncomingCallNumber
    {
        void operator()(std::shared_ptr<bluetooth::BaseProfileManager> profileManager,
                        bluetooth::event::IncomingCallNumber evt)
        {
            profileManager->setIncomingCallNumber(evt.number);
        }
    } constexpr IncomingCallNumber;

    struct OutgoingCallStarted
    {
        void operator()(std::shared_ptr<bluetooth::BaseProfileManager> profileManager,
                        bluetooth::event::OutgoingCallStarted evt)
        {
            profileManager->outgoingCallStarted(evt.number);
        }
    } constexpr OutgoingCallStarted;

    struct IncomingCallAnswered
    {
        void operator()(std::shared_ptr<bluetooth::BaseProfileManager> profileManager)
        {
            profileManager->incomingCallAnswered();
        }
    } constexpr IncomingCallAnswered;

    struct OutgoingCallAnswered
    {
        void operator()(std::shared_ptr<bluetooth::BaseProfileManager> profileManager)
        {
            profileManager->outgoingCallAnswered();
        }
    } constexpr OutgoingCallAnswered;

    struct CallTerminated
    {
        void operator()(std::shared_ptr<bluetooth::BaseProfileManager> profileManager)
        {
            profileManager->callTerminated();
        }
    } constexpr CallTerminated;

    struct CallMissed
    {
        void operator()(std::shared_ptr<bluetooth::BaseProfileManager> profileManager)
        {
            profileManager->callMissed();
        }
    } constexpr CallMissed;

    struct SignalStrength
    {
        void operator()(std::shared_ptr<bluetooth::BaseProfileManager> profileManager,
                        bluetooth::event::SignalStrengthData evt)
        {
            profileManager->setSignalStrengthData(evt.strength);
        }
    } constexpr SignalStrength;

    struct SetOperatorName
    {
        void operator()(std::shared_ptr<bluetooth::BaseProfileManager> profileManager,
                        bluetooth::event::OperatorNameData evt)
        {
            profileManager->setOperatorNameData(bluetooth::OperatorName(evt.name));
        }
    } constexpr SetOperatorName;

    struct SetBatteryLevel
    {
        void operator()(std::shared_ptr<bluetooth::BaseProfileManager> profileManager,
                        bluetooth::event::BatteryLevelData evt)
        {
            profileManager->setBatteryLevelData(evt.level);
        }
    } constexpr SetBatteryLevel;

    struct SetNetworkStatus
    {
        void operator()(std::shared_ptr<bluetooth::BaseProfileManager> profileManager,
                        bluetooth::event::NetworkStatusData evt)
        {
            profileManager->setNetworkStatusData(evt.status);
        }
    } constexpr SetNetworkStatus;

    struct StartAudio
    {
        void operator()(std::shared_ptr<bluetooth::BaseProfileManager> profileManager)
        {
            profileManager->start();
        }
    } constexpr StartAudio;

    struct StopAudio
    {
        void operator()(std::shared_ptr<bluetooth::BaseProfileManager> profileManager)
        {
            profileManager->stop();
        }
    } constexpr StopAudio;

    struct Setup
    {
      public:
        auto operator()() const
        {
            using namespace sml;
            return make_transition_table(*"Setup"_s + on_entry<_>[!isInit] / (InitDevicesList, InitDriver, PostInit),
                                         "Setup"_s / StartDriver = X);
        }
    };

    struct Call
    {
        auto operator()() const
        {
            using namespace sml;
            // clang-format off
                return make_transition_table(
                        *"CallSetup"_s + sml::event<bluetooth::event::IncomingCallStarted> / IncomingCallStarted = "CallRinging"_s,
                        "CallSetup"_s + sml::event<bluetooth::event::IncomingCallNumber> / (IncomingCallStarted, IncomingCallNumber) = "CallRinging"_s,
                        "CallSetup"_s + sml::event<bluetooth::event::OutgoingCallStarted> / OutgoingCallStarted = "CallInitiated"_s,

                        "CallRinging"_s + sml::event<bluetooth::event::IncomingCallNumber> / IncomingCallNumber = "CallRinging"_s,
                        "CallRinging"_s + sml::event<bluetooth::event::CallAnswered> / IncomingCallAnswered = "CallInProgress"_s,
                        "CallRinging"_s + sml::event<bluetooth::event::CallTerminated> / CallTerminated = "CallEnded"_s,
                        "CallRinging"_s + sml::event<bluetooth::event::CallMissed> / CallMissed = "CallEnded"_s,

                        "CallInitiated"_s + sml::event<bluetooth::event::CallAnswered> / OutgoingCallAnswered = "CallInProgress"_s,
                        "CallInitiated"_s + sml::event<bluetooth::event::CallTerminated> / CallTerminated = "CallEnded"_s,

                        "CallInProgress"_s + sml::event<bluetooth::event::CallTerminated> / CallTerminated = "CallEnded"_s,

                        "CallEnded"_s = X
                );
            // clang-format on
        }
    };

    class Idle;
    struct On
    {
        auto operator()() const
        {
            const auto forwardEvent = [](const auto &ev, auto &sm, auto &deps, auto &subs) {
                sm.process_event(ev, deps, subs);
            };

            using namespace sml;
            // clang-format off
                return make_transition_table(
                        *state<Idle> + sml::event<bluetooth::event::StartScan> / HandleOn = state<Idle>,
                        state<Idle> + sml::event<bluetooth::event::StopScan> / HandleOff = state<Idle>,
                        state<Idle> + sml::event<bluetooth::event::Pair> / HandlePair = state<Idle>,
                        state<Idle> + sml::event<bluetooth::event::Unpair>[Connected] / (HandleDisconnect, HandleUnpair, HandleDrop) = state<Idle>,
                        state<Idle> + sml::event<bluetooth::event::Unpair>[not Connected] / (HandleUnpair, HandleDrop) = state<Idle>,

                        state<Idle> + sml::event<bluetooth::event::VisibilityOn> / HandleSetVisibility = state<Idle>,
                        state<Idle> + sml::event<bluetooth::event::VisibilityOff> / HandleUnsetVisibility = state<Idle>,
                        state<Idle> + sml::event<bluetooth::event::Connect> / HandleConnect = state<Idle>,
                        state<Idle> + sml::event<bluetooth::event::Disconnect> / HandleDisconnect = state<Idle>,

                        state<Idle> + sml::event<bluetooth::event::SignalStrengthData> / SignalStrength = state<Idle>,
                        state<Idle> + sml::event<bluetooth::event::OperatorNameData>/  SetOperatorName = state<Idle>,
                        state<Idle> + sml::event<bluetooth::event::BatteryLevelData>/ SetBatteryLevel = state<Idle>,
                        state<Idle> + sml::event<bluetooth::event::NetworkStatusData> / SetNetworkStatus = state<Idle>,
                        state<Idle> + sml::event<bluetooth::event::StartStream>/ StartAudio = state<Idle>,
                        state<Idle> + sml::event<bluetooth::event::StopStream>/ StopAudio = state<Idle>,

                        state<Idle> + sml::event<bluetooth::event::IncomingCallStarted> / forwardEvent = state<Call>,
                        state<Idle> + sml::event<bluetooth::event::IncomingCallNumber> / forwardEvent = state<Call>,
                        state<Idle> + sml::event<bluetooth::event::OutgoingCallStarted> / forwardEvent = state<Call>,

                        state<Call> = state<Idle> // this one is needed to go out from Call substate properly!
                );
            // clang-format on
        }
    };

    struct ExceptionHandler
    {
        void operator()(const std::runtime_error &err)
        {
            LOG_FATAL("EXCEPTION %s", err.what());
        }
    } constexpr ExceptionHandler;

    struct TurnOff
    {
        void operator()(std::shared_ptr<AbstractDriver> driver,
                        std::shared_ptr<bluetooth::BaseProfileManager> profileManager)
        {
            profileManager->deInit();
            driver->stop();
        };
    } constexpr TurnOff;

    class StateMachine
    {
      public:
        auto operator()() const
        {
            auto printInitError       = [](const InitializationError &error) { LOG_ERROR("%s", error.what()); };
            auto printProcessingError = [](const ProcessingError &error) { LOG_ERROR("%s", error.what()); };

            using namespace sml;
            // clang-format off
                return make_transition_table(*"Off"_s + sml::event<bluetooth::event::PowerOn> = state<Setup>,
                                             state<Setup>[isInit] = state<On>,
                                             state<Setup>[not isInit] = "Restart"_s,
                                             state<Setup> + exception<InitializationError> / printInitError = "Off"_s,

                                             state<On> + sml::event<bluetooth::event::PowerOff> / TurnOff = "Off"_s,
                                             state<On> + exception<ProcessingError> / ( printProcessingError, TurnOff ) = "Restart"_s,
                                             state<On> + sml::event<bluetooth::event::ShutDown> / TurnOff = X,

                                             "Restart"_s = state<Setup>,
                                             "Restart"_s + sml::event<bluetooth::event::ShutDown> /TurnOff = X,

                                            *("ExceptionsHandling"_s) + exception<std::runtime_error> / ExceptionHandler = "Off"_s,
                                            "ExceptionsHandling"_s + exception<std::runtime_error> / ExceptionHandler    = "Off"_s,

                                             "Off"_s + sml::event<bluetooth::event::ShutDown> = X);
            // clang-format on
        }
    };

    class StatefulController::Impl
    {
      public:
        Impl() = delete;
        Impl(std::shared_ptr<AbstractDriver> driver,
             std::shared_ptr<AbstractCommandHandler> handler,
             DeviceRegistrationFunction registerDevice,
             std::shared_ptr<bluetooth::SettingsHolder> settings,
             std::shared_ptr<std::vector<Devicei>> pairedDevices,
             std::shared_ptr<bluetooth::BaseProfileManager> profileManager);
        using SM = sml::sm<StateMachine, boost::sml::logger<Logger>>;
        SM sm;
    };

    StatefulController::Impl::Impl(std::shared_ptr<AbstractDriver> driver,
                                   std::shared_ptr<AbstractCommandHandler> handler,
                                   DeviceRegistrationFunction registerDevice,
                                   std::shared_ptr<bluetooth::SettingsHolder> settings,
                                   std::shared_ptr<std::vector<Devicei>> pairedDevices,
                                   std::shared_ptr<bluetooth::BaseProfileManager> profileManager)
        : sm{Logger{}, driver, handler, registerDevice, InitializationState{}, settings, pairedDevices, profileManager}
    {}
} // namespace bluetooth
