// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WorkerController.hpp"

#include "Device.hpp"
#include "interface/profiles/ProfileManager.hpp"

#include <log/log.hpp>
#define BOOST_SML_CFG_DISABLE_MIN_SIZE // GCC10 fix
#include <boost/sml.hpp>
#include <sml-utils/Logger.hpp>
#include <magic_enum.hpp>
#include <stdexcept>
#include "command/event/Events.hpp"
#include "service-bluetooth/SettingsHolder.hpp"

namespace bluetooth
{
    namespace sml = boost::sml;

    namespace
    {
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

        struct Setup
        {
          public:
            auto operator()() const
            {
                auto isInit = [](InitializationState &data) { return data.isInitDone; };
                auto init   = [](std::shared_ptr<AbstractDriver> &driver) {
                    if (const auto status = driver->init(); status != Error::Success) {
                        throw InitializationError{"Unable to initialize a bluetooth driver."};
                    }
                };
                auto setup = [](DeviceRegistrationFunction &registerDevice, InitializationState &data) {
                    if (const auto status = registerDevice(); status != Error::Success) {
                        throw InitializationError{"Unable to initialize bluetooth"};
                    }
                    data.isInitDone = true;
                };
                auto startDriver = [](std::shared_ptr<AbstractDriver> &driver) {
                    if (const auto status = driver->run(); status != Error::Success) {
                        throw InitializationError{"Unable to run the bluetooth driver"};
                    }
                };

                using namespace sml;
                // clang-format off
                return make_transition_table(*"Setup"_s / startDriver = "StartingDriver"_s,
                                             "Setup"_s + on_entry<_> [ !isInit ] / ( init, setup ),
                                             "StartingDriver"_s = X);
                // clang-format on
            }
        };

        struct
        {
            void operator()(const std::shared_ptr<AbstractCommandHandler> &handler)
            {
                handler->scan();
            }
        } constexpr HandleOn;

        struct
        {
            void operator()(const std::shared_ptr<AbstractCommandHandler> &handler)
            {
                handler->stopScan();
            }
        } constexpr HandleOff;

        struct
        {
            void operator()(const std::shared_ptr<AbstractCommandHandler> &handler, const bt::evt::Pair &event)
            {
                handler->pair(event.device);
            }
        } constexpr HandlePair;

        struct
        {
            bool operator()(const std::shared_ptr<bluetooth::SettingsHolder> &settings, bt::evt::Unpair evt)
            {
                auto deviceAddr =
                    std::visit(bluetooth::StringVisitor(), settings->getValue(bluetooth::Settings::ConnectedDevice));
                return static_cast<bool>(deviceAddr == bd_addr_to_str(evt.device.address));
            }
        } constexpr Connected;

        struct
        {
            void operator()(const std::shared_ptr<AbstractDriver> &driver,
                            const bt::evt::Unpair &event,
                            const std::shared_ptr<bluetooth::SettingsHolder> &settings,
                            const std::shared_ptr<std::vector<Devicei>> &pairedDevices)
            {
                driver->unpair(event.device);

                auto position = std::find_if(pairedDevices->begin(), pairedDevices->end(), [&](const Devicei &device) {
                    return !bd_addr_cmp(event.device.address, device.address);
                });
                if (position != pairedDevices->end()) {
                    pairedDevices->erase(position);
                    settings->setValue(bluetooth::Settings::BondedDevices,
                                       SettingsSerializer::toString(*pairedDevices));
                    LOG_INFO("Device removed from paired devices list");
                }
            }
        } constexpr HandleUnpair;

        struct
        {
            void operator()(const std::shared_ptr<AbstractCommandHandler> &handler)
            {
                handler->disconnectAudioConnection();
            }

        } constexpr HandleDisconnect;

        struct
        {
            void operator()(const std::shared_ptr<AbstractCommandHandler> &handler)
            {
                handler->setVisibility(true);
            }

        } constexpr HandleSetVisibility;

        struct
        {
            void operator()(const std::shared_ptr<AbstractCommandHandler> &handler)
            {
                handler->setVisibility(false);
            }

        } constexpr HandleUnsetVisibility;

        struct
        {
            void operator()(const std::shared_ptr<AbstractCommandHandler> &handler, bt::evt::ConnectAudio evt)
            {
                handler->establishAudioConnection(evt.device);
            }

        } constexpr EstablishAudioConnection;

        struct
        {
            void operator()(std::shared_ptr<bluetooth::ProfileManager> profileManager)
            {
                profileManager->startRinging();
            }

        } constexpr StartRinging;

        struct
        {
            void operator()(std::shared_ptr<bluetooth::ProfileManager> profileManager)
            {
                profileManager->stopRinging();
            }

        } constexpr StopRinging;

        struct
        {
            void operator()(std::shared_ptr<bluetooth::ProfileManager> profileManager, bt::evt::ConnectAudio evt)
            {
                profileManager->initializeCall();
            }

        } constexpr InitializeCall;

        struct
        {
            void operator()(std::shared_ptr<bluetooth::ProfileManager> profileManager)
            {
                profileManager->callAnswered();
            }

        } constexpr CallAnswered;

        struct
        {
            void operator()(std::shared_ptr<bluetooth::ProfileManager> profileManager)
            {
                profileManager->terminateCall();
            }

        } constexpr TerminateCall;

        struct
        {
            void operator()(std::shared_ptr<bluetooth::ProfileManager> profileManager, bt::evt::CallStarted evt)
            {
                profileManager->callStarted(evt.number);
            }

        } constexpr CallStarted;

        struct
        {
            void operator()(std::shared_ptr<bluetooth::ProfileManager> profileManager, bt::evt::IncomingCallNumber evt)
            {
                profileManager->setIncomingCallNumber(evt.number);
            }

        } constexpr IncomingCall;

        struct
        {
            void operator()(std::shared_ptr<bluetooth::ProfileManager> profileManager, bt::evt::SignalStrengthData evt)
            {
                profileManager->setSignalStrengthData(evt.strength);
            }

        } constexpr SignalStrength;

        struct
        {
            void operator()(std::shared_ptr<bluetooth::ProfileManager> profileManager, bt::evt::OperatorNameData evt)
            {
                profileManager->setOperatorNameData(evt.name);
            }

        } constexpr SetOperatorName;

        struct
        {
            void operator()(std::shared_ptr<bluetooth::ProfileManager> profileManager, bt::evt::BatteryLevelData evt)
            {
                profileManager->setBatteryLevelData(evt.level);
            }

        } constexpr SetBatteryLevel;

        struct
        {
            void operator()(std::shared_ptr<bluetooth::ProfileManager> profileManager, bt::evt::NetworkStatusData evt)
            {
                profileManager->setNetworkStatusData(evt.status);
            }
        } constexpr SetNetworkStatus;

        struct
        {
            void operator()(std::shared_ptr<bluetooth::ProfileManager> profileManager)
            {
                profileManager->start();
            }

        } constexpr StartAudio;

        struct
        {
            void operator()(std::shared_ptr<bluetooth::ProfileManager> profileManager)
            {
                profileManager->stop();
            }

        } constexpr StopAudio;

        struct On
        {
            auto operator()() const
            {
                auto isInit        = [](InitializationState &data) { return data.isInitDone; };

                using namespace sml;
                // clang-format off
                return make_transition_table(
                                            *"Idle"_s + event<bt::evt::StartScan>[isInit] / HandleOn = "Idle"_s,
                                            "Idle"_s + event<bt::evt::StopScan>[isInit] / HandleOff = "Idle"_s,
                                            "Idle"_s + event<bt::evt::Pair>[isInit] / HandlePair = "Idle"_s,
                                            "Idle"_s + event<bt::evt::Unpair>[isInit and Connected] / (HandleDisconnect, HandleUnpair) = "Idle"_s,
                                            "Idle"_s + event<bt::evt::Unpair>[isInit] / HandleUnpair = "Idle"_s,

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

        struct InitDevicesList
        {
            void operator()(const std::shared_ptr<bluetooth::SettingsHolder> &settings,
                            const std::shared_ptr<std::vector<Devicei>> &pairedDevices)
            {
                auto bondedDevicesStr =
                    std::visit(bluetooth::StringVisitor(), settings->getValue(bluetooth::Settings::BondedDevices));
                pairedDevices->clear();
                auto devices = SettingsSerializer::fromString(bondedDevicesStr);
                pairedDevices->assign(devices.begin(), devices.end());
            }
        } constexpr InitDevicesList;

        class StateMachine
        {
          public:
            auto operator()() const
            {
                auto turnOff              = [](std::shared_ptr<AbstractDriver> &driver) { driver->stop(); };
                auto printInitError       = [](const InitializationError &error) { LOG_ERROR("%s", error.what()); };
                auto printProcessingError = [](const ProcessingError &error) { LOG_ERROR("%s", error.what()); };

                using namespace sml;
                // clang-format off
                return make_transition_table(*"Off"_s + event<bt::evt::PowerOn> = state<Setup>,
                                             state<Setup> / InitDevicesList = state<On>,
                                             state<Setup> + exception<InitializationError> / printInitError = "Off"_s,
                                             state<On> + event<bt::evt::PowerOff> / turnOff = "Off"_s,
                                             state<On> + exception<ProcessingError> / ( printProcessingError, turnOff ) = "Restart"_s,
                                             "Restart"_s = state<Setup>,

                                             state<Setup> + event<bt::evt::ShutDown> / turnOff = X,
                                             state<On> + event<bt::evt::ShutDown> / turnOff = X,
                                             "Restart"_s + event<bt::evt::ShutDown> /turnOff = X,

                                             "Off"_s + event<bt::evt::ShutDown> = X);
                // clang-format on
            }
        };
    } // namespace

    class StatefulController::Impl
    {
      public:
        Impl(std::shared_ptr<AbstractDriver> &&driver,
             std::shared_ptr<AbstractCommandHandler> &&handler,
             DeviceRegistrationFunction &&registerDevice,
             std::shared_ptr<bluetooth::SettingsHolder> &&settings,
             std::shared_ptr<std::vector<Devicei>> &&pairedDevices);

        Logger logger;

        using SM = sml::sm<StateMachine, boost::sml::logger<Logger>>;
        SM sm;
    };

    StatefulController::Impl::Impl(std::shared_ptr<AbstractDriver> &&driver,
                                   std::shared_ptr<AbstractCommandHandler> &&handler,
                                   DeviceRegistrationFunction &&registerDevice,
                                   std::shared_ptr<bluetooth::SettingsHolder> &&settings,
                                   std::shared_ptr<std::vector<Devicei>> &&pairedDevices)
        : sm{logger,
             std::move(driver),
             std::move(handler),
             std::move(registerDevice),
             InitializationState{},
             std::move(settings),
             std::move(pairedDevices)}
    {}

    StatefulController::StatefulController(std::shared_ptr<AbstractDriver> &&driver,
                                           std::shared_ptr<AbstractCommandHandler> &&handler,
                                           DeviceRegistrationFunction &&registerDevice,
                                           std::shared_ptr<bluetooth::SettingsHolder> &&settings,
                                           std::shared_ptr<std::vector<Devicei>> &&pairedDevices)
        : pimpl(std::make_unique<Impl>(std::move(driver),
                                       std::move(handler),
                                       std::move(registerDevice),
                                       std::move(settings),
                                       std::move(pairedDevices)))
    {}

    StatefulController::~StatefulController() noexcept = default;

    void StatefulController::handle(const bt::evt::Base &evt)
    {
        pimpl->sm.process_event(evt);
    }
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
        using namespace sml;
        return pimpl->sm.is(X);
    }
} // namespace bluetooth
