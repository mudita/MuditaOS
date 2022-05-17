// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WorkerController.hpp"

#include "Device.hpp"
#include "interface/profiles/ProfileManager.hpp"

#include <log/log.hpp>
// TODO test if it does anything
// #define BOOST_SML_CFG_DISABLE_MIN_SIZE // GCC10 fix
#include <boost/sml.hpp>
#include <sml-utils/Logger.hpp>
#include <magic_enum.hpp>
#include <stdexcept>
#include "command/event/Events.hpp"
#include "service-bluetooth/SettingsHolder.hpp"

namespace bluetooth
{
    namespace sml = boost::sml;

    //    namespace
    //    {
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
            if (const auto status = driver->init(); status != Error::Success) {
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

    struct Preinit
    {
        // TODO shoundn't this be in driver?
        void operator()(DeviceRegistrationFunction registerDevice, InitializationState &data)
        {
            if (const auto status = registerDevice(); status != Error::Success) {
                throw InitializationError{"Unable to initialize bluetooth"};
            }
            data.isInitDone = true;
        }
    } constexpr Preinit;

    struct
    {
        void operator()(std::shared_ptr<AbstractDriver> driver)
        {
            printf("Start driver\n");
            if (const auto status = driver->run(); status != Error::Success) {
                throw InitializationError{"Unable to run the bluetooth driver"};
            }
        }
    } static StartDriver;

    struct Setup
    {
      public:
        auto operator()() const
        {
            using namespace sml;
            return make_transition_table(*"Setup"_s + on_entry<_>[!isInit] / (InitDevicesList, Preinit, InitDriver),
                                         "Setup"_s / StartDriver = X);
        }
    };

    struct
    {
        void operator()(const std::shared_ptr<AbstractCommandHandler> handler)
        {
            handler->scan();
        }
    } constexpr HandleOn;

    struct
    {
        void operator()(const std::shared_ptr<AbstractCommandHandler> handler)
        {
            handler->stopScan();
        }
    } constexpr HandleOff;

    struct
    {
        void operator()(const std::shared_ptr<AbstractCommandHandler> handler, const bt::evt::Pair &event)
        {
            handler->pair(event.device);
        }
    } constexpr HandlePair;

    struct
    {
        bool operator()(std::shared_ptr<bluetooth::SettingsHolder> settings, bt::evt::Unpair evt)
        {
            auto deviceAddr =
                std::visit(bluetooth::StringVisitor(), settings->getValue(bluetooth::Settings::ConnectedDevice));
            return static_cast<bool>(deviceAddr == bd_addr_to_str(evt.device.address));
        }
    } constexpr Connected;

    struct
    {
        void operator()(std::shared_ptr<AbstractDriver> driver, const bt::evt::Unpair &event
                        //,
                        // const std::shared_ptr<bluetooth::SettingsHolder> &settings
                        //,const std::shared_ptr<std::vector<Devicei>> &pairedDevices)
        )
        {
            driver->unpair(event.device);
        }
    } constexpr HandleUnpair;

    struct HandleDrop
    {
        void operator()(const bt::evt::Unpair &event,
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

    struct
    {
        void operator()(const std::shared_ptr<AbstractCommandHandler> handler)
        {
            handler->disconnectAudioConnection();
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

    struct EstablishAudioConnection
    {
        void operator()(std::shared_ptr<AbstractCommandHandler> handler, bt::evt::ConnectAudio evt)
        {
            handler->establishAudioConnection(evt.device);
        }

    } constexpr EstablishAudioConnection;

    struct StartRinging
    {
        void operator()(std::shared_ptr<bluetooth::BaseProfileManager> profileManager)
        {
            profileManager->startRinging();
        }
    } constexpr StartRinging;

    struct StopRinging
    {
        void operator()(std::shared_ptr<bluetooth::BaseProfileManager> profileManager)
        {
            profileManager->stopRinging();
        }
    } constexpr StopRinging;

    struct InitializeCall
    {
        void operator()(std::shared_ptr<bluetooth::BaseProfileManager> profileManager)
        {
            profileManager->initializeCall();
        }
    } constexpr InitializeCall;

    struct CallAnswered
    {
        void operator()(std::shared_ptr<bluetooth::BaseProfileManager> profileManager)
        {
            profileManager->callAnswered();
        }
    } constexpr CallAnswered;

    struct TerminateCall
    {
        void operator()(std::shared_ptr<bluetooth::BaseProfileManager> profileManager)
        {
            profileManager->terminateCall();
        }
    } constexpr TerminateCall;

    struct CallStarted
    {
        void operator()(std::shared_ptr<bluetooth::BaseProfileManager> profileManager, bt::evt::CallStarted evt)
        {
            profileManager->callStarted(evt.number);
        }
    } constexpr CallStarted;

    struct IncomingCall
    {
        void operator()(std::shared_ptr<bluetooth::BaseProfileManager> profileManager, bt::evt::IncomingCallNumber evt)
        {
            profileManager->setIncomingCallNumber(evt.number);
        }
    } constexpr IncomingCall;

    struct SignalStrength
    {
        void operator()(std::shared_ptr<bluetooth::BaseProfileManager> profileManager, bt::evt::SignalStrengthData evt)
        {
            profileManager->setSignalStrengthData(evt.strength);
        }
    } constexpr SignalStrength;

    struct SetOperatorName
    {
        void operator()(const std::shared_ptr<bluetooth::BaseProfileManager> &profileManager,
                        bt::evt::OperatorNameData evt)
        {
            profileManager->setOperatorNameData(bluetooth::OperatorName(evt.name));
        }
    } constexpr SetOperatorName;

    struct SetBatteryLevel
    {
        void operator()(std::shared_ptr<bluetooth::BaseProfileManager> profileManager, bt::evt::BatteryLevelData evt)
        {
            profileManager->setBatteryLevelData(evt.level);
        }
    } constexpr SetBatteryLevel;

    struct
    {
        void operator()(std::shared_ptr<bluetooth::BaseProfileManager> profileManager, bt::evt::NetworkStatusData evt)
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

    struct On
    {
        auto operator()() const
        {
            auto isInit = [](InitializationState &data) { return data.isInitDone; };

            using namespace sml;
            // clang-format off
                return make_transition_table(
                       *"Idle"_s + event<bt::evt::StartScan>[isInit] / HandleOn = "Idle"_s,
                       "Idle"_s + event<bt::evt::StopScan>[isInit] / HandleOff = "Idle"_s,
                       "Idle"_s + event<bt::evt::Pair>[isInit] / HandlePair = "Idle"_s,
                       "Idle"_s + event<bt::evt::Unpair>[isInit and Connected] / (HandleDisconnect, HandleUnpair) = "Idle"_s,
                       "Idle"_s + event<bt::evt::Unpair>[isInit] / (HandleUnpair, HandleDrop) = "Idle"_s,

                       "Idle"_s + event<bt::evt::VisibilityOn> / HandleSetVisibility = "Idle"_s,
                       "Idle"_s + event<bt::evt::VisibilityOff> / HandleUnsetVisibility = "Idle"_s,
                       "Idle"_s + event<bt::evt::ConnectAudio> / EstablishAudioConnection = "Idle"_s,

                       "Idle"_s + event<bt::evt::StartRinging> / StartRinging = "Idle"_s,
                       "Idle"_s + event<bt::evt::StopRinging> / StopRinging = "Idle"_s,
                       "Idle"_s + event<bt::evt::StartRouting> /InitializeCall = "Idle"_s,
                       "Idle"_s + event<bt::evt::CallAnswered> / CallAnswered = "Idle"_s,
                       "Idle"_s + event<bt::evt::CallTerminated> / TerminateCall = "Idle"_s,
                       "Idle"_s + event<bt::evt::CallStarted>/ CallStarted = "Idle"_s,
                       "Idle"_s + event<bt::evt::IncomingCallNumber> / IncomingCall= "Idle"_s,
                       "Idle"_s + event<bt::evt::SignalStrengthData> / SignalStrength = "Idle"_s,
                       "Idle"_s + event<bt::evt::OperatorNameData>/  SetOperatorName = "Idle"_s,
                       "Idle"_s + event<bt::evt::BatteryLevelData>/ SetBatteryLevel = "Idle"_s,
                       "Idle"_s + event<bt::evt::NetworkStatusData> / SetNetworkStatus = "Idle"_s,
                       "Idle"_s + event<bt::evt::StartStream>/ StartAudio = "Idle"_s,
                       "Idle"_s + event<bt::evt::StopStream>/ StopAudio = "Idle"_s
                       );
            // clang-format on
        }
    };

    struct
    {
        void operator()(const std::runtime_error &err)
        {
            LOG_FATAL("EXCEPTION %s", err.what());
        }
    } constexpr ExceptionHandler;

    class StateMachine
    {
      public:
        auto operator()() const
        {
            auto turnOff              = [](std::shared_ptr<AbstractDriver> driver) { driver->stop(); };
            auto printInitError       = [](const InitializationError &error) { LOG_ERROR("%s", error.what()); };
            auto printProcessingError = [](const ProcessingError &error) { LOG_ERROR("%s", error.what()); };

            using namespace sml;
            // clang-format off
                return make_transition_table(*"Off"_s + event<bt::evt::PowerOn> = state<Setup>,
                                             state<Setup> = state<On>,
                                             state<Setup> + exception<InitializationError> / printInitError = "Off"_s,

                                             state<On> + event<bt::evt::PowerOff> / turnOff = "Off"_s,
                                             state<On> + exception<ProcessingError> / ( printProcessingError, turnOff ) = "Restart"_s,
                                             state<On> + event<bt::evt::ShutDown> / turnOff = X,

                                             "Restart"_s = state<Setup>,
                                             "Restart"_s + event<bt::evt::ShutDown> /turnOff = X,

                                            *("ExceptionsHandling"_s) + exception<std::runtime_error> / ExceptionHandler = "Off"_s,
                                            "ExceptionsHandling"_s + exception<std::runtime_error> / ExceptionHandler    = "Off"_s,

                                             "Off"_s + event<bt::evt::ShutDown> = X);
            // clang-format on
        }
    };
    //    } // namespace

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

    StatefulController::StatefulController(std::shared_ptr<AbstractDriver> driver,
                                           std::shared_ptr<AbstractCommandHandler> handler,
                                           DeviceRegistrationFunction registerDevice,
                                           std::shared_ptr<bluetooth::SettingsHolder> settings,
                                           std::shared_ptr<std::vector<Devicei>> pairedDevices,
                                           std::shared_ptr<bluetooth::BaseProfileManager> profileManager)
        : pimpl(std::make_unique<Impl>(driver, handler, registerDevice, settings, pairedDevices, profileManager))
    {
        // printf("---------- registered: 0x%X\n", driver.get());
    }

    StatefulController::StatefulController(StatefulController &&other) noexcept
    {
        this->pimpl = std::move(other.pimpl);
    }

    StatefulController &StatefulController::operator=(StatefulController &&other) noexcept
    {
        this->pimpl = std::move(other.pimpl);
        return *this;
    }

    StatefulController::~StatefulController() noexcept = default;

    //--------------------------------------------------------------------
    // entry to dispatch to call handle (double visitor -> double dispatch)
    void StatefulController::handle(const bt::evt::Base &evt)
    {
        evt.dispatch(this);
    }

    //-----------------------------------------
    // all `handle` code below is casual visitor

    void StatefulController::handle(const bt::evt::StartScan &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::StopScan &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::GetDevicesAvailable &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::VisibilityOn &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::VisibilityOff &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::ConnectAudio &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::DisconnectAudio &evt)
    {
        pimpl->sm.process_event(evt);
    };
    // TODO split TurnOn and PowerOn?
    void StatefulController::handle(const bt::evt::PowerOn &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::PowerOff &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::ShutDown &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::Pair &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::Unpair &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::StartRinging &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::StopRinging &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::StartRouting &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::StartStream &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::StopStream &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::CallAnswered &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::CallTerminated &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::CallStarted &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::IncomingCallNumber &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::SignalStrengthData &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::OperatorNameData &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::BatteryLevelData &evt)
    {
        pimpl->sm.process_event(evt);
    };
    void StatefulController::handle(const bt::evt::NetworkStatusData &evt)
    {
        pimpl->sm.process_event(evt);
    };

    auto StatefulController::isTerminated() const -> bool
    {
        return pimpl->sm.is(sml::X);
    }
} // namespace bluetooth
