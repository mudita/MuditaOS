// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WorkerController.hpp"

#include "Device.hpp"
#include "interface/profiles/ProfileManager.hpp"

#include <log.hpp>
#define BOOST_SML_CFG_DISABLE_MIN_SIZE // GCC10 fix
#include <boost/sml.hpp>
#include <magic_enum.hpp>
#include <stdexcept>

namespace bluetooth
{
    namespace sml = boost::sml;

    namespace
    {
        struct TurnOn
        {};
        struct TurnOff
        {};
        struct ShutDown
        {};
        struct ProcessCommand
        {
            Command command;
        };

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

        struct On
        {
            auto operator()() const
            {
                auto isInit        = [](InitializationState &data) { return data.isInitDone; };
                auto handleCommand = [](std::shared_ptr<AbstractCommandHandler> &processor,
                                        const ProcessCommand &processCommand) {
                    if (const auto status = processor->handle(processCommand.command); status != Error::Success) {
                        throw ProcessingError{"Failed to process command"};
                    }
                };

                using namespace sml;
                // clang-format off
                return make_transition_table(*"Idle"_s + event<ProcessCommand> [ isInit ] / handleCommand = "Processing"_s,
                                             "Processing"_s = "Idle"_s);
                // clang-format on
            }
        };

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
                return make_transition_table(*"Off"_s + event<TurnOn> = state<Setup>,
                                             state<Setup> = state<On>,
                                             state<Setup> + exception<InitializationError> / printInitError = "Off"_s,
                                             state<On> + event<TurnOff> / turnOff = "Off"_s,
                                             state<On> + exception<ProcessingError> / ( printProcessingError, turnOff ) = "Restart"_s,
                                             "Restart"_s = state<Setup>,
                                             "Off"_s + event<ShutDown> = X);
                // clang-format on
            }
        };
    } // namespace

    class StatefulController::Impl
    {
      public:
        Impl(std::shared_ptr<AbstractDriver> &&driver,
             std::shared_ptr<AbstractCommandHandler> &&handler,
             DeviceRegistrationFunction &&registerDevice);

        using SM = sml::sm<StateMachine>;
        SM sm;
    };

    StatefulController::Impl::Impl(std::shared_ptr<AbstractDriver> &&driver,
                                   std::shared_ptr<AbstractCommandHandler> &&handler,
                                   DeviceRegistrationFunction &&registerDevice)
        : sm{std::move(driver), std::move(handler), std::move(registerDevice), InitializationState{}}
    {}

    StatefulController::StatefulController(std::shared_ptr<AbstractDriver> &&driver,
                                           std::shared_ptr<AbstractCommandHandler> &&handler,
                                           DeviceRegistrationFunction &&registerDevice)
        : pimpl(std::make_unique<Impl>(std::move(driver), std::move(handler), std::move(registerDevice)))
    {}

    StatefulController::~StatefulController() noexcept = default;

    void StatefulController::turnOn()
    {
        pimpl->sm.process_event(TurnOn{});
    }

    void StatefulController::turnOff()
    {
        pimpl->sm.process_event(TurnOff{});
    }

    void StatefulController::shutdown()
    {
        if (isOn()) {
            turnOff();
        }
        pimpl->sm.process_event(ShutDown{});
    }

    auto StatefulController::isOn() const -> bool
    {
        using namespace sml;
        return !pimpl->sm.is("Off"_s) && !isTerminated();
    }

    auto StatefulController::isTerminated() const -> bool
    {
        using namespace sml;
        return pimpl->sm.is(X);
    }

    void StatefulController::processCommand(Command command)
    {
        LOG_INFO("Process command: %s", magic_enum::enum_name(command.getType()).data());
        pimpl->sm.process_event(ProcessCommand{command});
    }
} // namespace bluetooth
