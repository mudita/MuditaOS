// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryLevelCheck.hpp"
#include "service-evtmgr/BatteryMessages.hpp"
#include "SystemManager/Constants.hpp"
#include <agents/settings/SystemSettings.hpp>
#include <common_data/EventStore.hpp>
#include <Utils.hpp>

#define BOOST_SML_CFG_DISABLE_MIN_SIZE // GCC10 fix
#include <module-utils/sml/include/boost/sml.hpp>

namespace battery_level_check
{
    namespace
    {
        unsigned int batteryLevelCritical = 0;

        constexpr auto batteryShutdownLevel = 5;

        sys::Service *parentService = nullptr;

        std::shared_ptr<settings::Settings> settings = nullptr;

        void sendCriticalLevelMessage(bool charging)
        {
            auto levelCriticalMessage = std::make_shared<sevm::BatteryLevelCriticalMessage>(charging);
            parentService->bus.sendUnicast(std::move(levelCriticalMessage), service::name::system_manager);
        }

        void sendShutdownLevelMessage()
        {
            auto shutdownMessage = std::make_shared<sevm::BatteryShutdownLevelMessage>();
            parentService->bus.sendUnicast(std::move(shutdownMessage), service::name::system_manager);
        }

        void sendNormalLevelMessage()
        {
            auto levelNormalMessage = std::make_shared<sevm::BatteryLevelNormalMessage>();
            parentService->bus.sendUnicast(std::move(levelNormalMessage), service::name::system_manager);
        }

        namespace sml = boost::sml;

        // events
        struct criticalLevelCheck
        {};
        struct confirmState
        {};

        // guards
        struct isNormal
        {
            bool operator()() const
            {
                return Store::Battery::get().level >= batteryLevelCritical;
            }
        } isNormal;
        struct isCriticalNotCharging
        {
            bool operator()() const
            {
                return Store::Battery::get().level < batteryLevelCritical &&
                       Store::Battery::get().state != Store::Battery::State::Charging;
            }
        } isCriticalNotCharging;
        struct isCriticalCharging
        {
            bool operator()() const
            {
                return Store::Battery::get().level < batteryLevelCritical &&
                       Store::Battery::get().state == Store::Battery::State::Charging;
            }
        } isCriticalCharging;
        struct isShutdown
        {
            bool operator()() const
            {
                return Store::Battery::get().level < batteryShutdownLevel;
            }
        } isShutdown;

        // actions
        struct sendCriticalNotCharging
        {
            void operator()()
            {
                sendCriticalLevelMessage(false);
            }
        } sendCriticalNotCharging;
        struct sendCriticalCharging
        {
            void operator()()
            {
                sendCriticalLevelMessage(true);
            }
        } sendCriticalCharging;
        struct sendNormal
        {
            void operator()()
            {
                sendNormalLevelMessage();
            }
        } sendNormal;
        struct sendShutdown
        {
            void operator()()
            {
                sendShutdownLevelMessage();
            }
        } sendShutdown;

        struct StateMachine
        {
            auto operator()() const
            {
                using namespace sml;
                // clang-format off
                return make_transition_table(
                *"LevelNormal"_s + event<criticalLevelCheck> [ isCriticalNotCharging ] / sendCriticalNotCharging = "LevelCriticalNotCharging"_s,
                "LevelNormal"_s + event<criticalLevelCheck> [ isCriticalCharging ] / sendCriticalCharging = "LevelCriticalCharging"_s,
                "LevelCriticalNotCharging"_s + event<criticalLevelCheck> [ isCriticalCharging ] / sendCriticalCharging = "LevelCriticalCharging"_s,
                "LevelCriticalCharging"_s + event<criticalLevelCheck> [ isCriticalNotCharging ] / sendCriticalNotCharging = "LevelCriticalNotCharging"_s,
                "LevelCriticalNotCharging"_s + event<criticalLevelCheck> [ isNormal ] / sendNormal = "LevelNormal"_s,
                "LevelCriticalCharging"_s + event<criticalLevelCheck> [ isNormal ] / sendNormal = "LevelNormal"_s,
                "LevelCriticalNotCharging"_s + event<criticalLevelCheck> [ isShutdown ] / sendShutdown = "Shutdown"_s,
                "LevelNormal"_s + event<confirmState> / sendNormal = "LevelNormal"_s,
                "LevelCriticalNotCharging"_s + event<confirmState> / sendCriticalNotCharging = "LevelCriticalNotCharging"_s,
                "LevelCriticalCharging"_s + event<confirmState> / sendCriticalCharging = "LevelCriticalCharging"_s,
                "Shutdown"_s = X
                );
                // clang-format on
            }
        };

        std::unique_ptr<sml::sm<StateMachine>> sm;
    } // namespace

    void init(sys::Service *service, std::shared_ptr<settings::Settings> &setts)
    {
        sm            = std::make_unique<sml::sm<StateMachine>>();
        parentService = service;
        settings      = setts;
        settings->registerValueChange(
            settings::Battery::batteryCriticalLevel,
            [&](const std::string &value) { batteryLevelCritical = utils::getNumericValue<unsigned int>(value); },
            settings::SettingsScope::Global);
        checkBatteryLevelCritical();
    }

    void deinit()
    {
        settings->unregisterValueChange(settings::Battery::batteryCriticalLevel, settings::SettingsScope::Global);
        settings.reset();
        sm.reset();
    }

    void checkBatteryLevelCritical()
    {
        sm->process_event(criticalLevelCheck{});
    }

    void checkBatteryLevelCriticalWithConfirmation()
    {
        sm->process_event(confirmState{});
    }

    void setBatteryCriticalLevel(unsigned int level)
    {
        batteryLevelCritical = level;
        settings->setValue(
            settings::Battery::batteryCriticalLevel, utils::to_string(level), settings::SettingsScope::Global);
        checkBatteryLevelCritical();
    }

} // namespace battery_level_check
