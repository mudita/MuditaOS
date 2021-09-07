// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryLevelCheck.hpp"
#include "service-evtmgr/BatteryMessages.hpp"
#include "SystemManager/Constants.hpp"
#include <service-db/agents/settings/SystemSettings.hpp>
#include <EventStore.hpp>
#include <Utils.hpp>

#define BOOST_SML_CFG_DISABLE_MIN_SIZE // GCC10 fix
#include <boost/sml.hpp>

namespace battery_level_check
{
    namespace
    {
        unsigned int batteryLevelCritical = 10;

        constexpr auto batteryShutdownLevel = 5;

        sys::Service *parentService = nullptr;

        std::shared_ptr<settings::Settings> settings = nullptr;

        namespace sml = boost::sml;

        // events
        struct initialLevelCheck
        {};
        struct criticalLevelCheck
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
        struct setCriticalNotCharging
        {
            void operator()()
            {
                Store::Battery::modify().levelState = Store::Battery::LevelState::CriticalNotCharging;
            }
        } setCriticalNotCharging;
        struct setCriticalCharging
        {
            void operator()()
            {
                Store::Battery::modify().levelState = Store::Battery::LevelState::CriticalCharging;
            }
        } setCriticalCharging;
        struct setNormal
        {
            void operator()()
            {
                Store::Battery::modify().levelState = Store::Battery::LevelState::Normal;
            }
        } setNormal;
        struct setShutdown
        {
            void operator()()
            {
                Store::Battery::modify().levelState = Store::Battery::LevelState::Shutdown;
            }
        } setShutdown;

        struct sendStateChange
        {
            void operator()()
            {
                auto stateChangeMessage = std::make_shared<sevm::BatteryStateChangeMessage>();
                parentService->bus.sendUnicast(std::move(stateChangeMessage), service::name::system_manager);
            }
        } sendStateChange;

        struct StateMachine
        {
            auto operator()() const
            {
                using namespace sml;
                // clang-format off
                return make_transition_table(
                *"InitialCheck"_s + event<initialLevelCheck> [ isCriticalNotCharging && !isShutdown ] / setCriticalNotCharging = "LevelCriticalNotCharging"_s,
                "InitialCheck"_s + event<initialLevelCheck> [ isCriticalNotCharging && isShutdown ] / setShutdown = "Shutdown"_s,
                "InitialCheck"_s + event<initialLevelCheck> [ isCriticalCharging ] / setCriticalCharging = "LevelCriticalCharging"_s,
                "InitialCheck"_s + event<initialLevelCheck> [ isNormal ]  / setNormal = "LevelNormal"_s,
                "LevelNormal"_s + event<criticalLevelCheck> [ isCriticalNotCharging && !isShutdown ] / (setCriticalNotCharging, sendStateChange) = "LevelCriticalNotCharging"_s,
                "LevelNormal"_s + event<criticalLevelCheck> [ isCriticalNotCharging && isShutdown ] / (setShutdown, sendStateChange) = "Shutdown"_s,
                "LevelNormal"_s + event<criticalLevelCheck> [ isCriticalCharging ] / (setCriticalCharging, sendStateChange) = "LevelCriticalCharging"_s,
                "LevelCriticalNotCharging"_s + event<criticalLevelCheck> [ isNormal ] / (setNormal, sendStateChange) = "LevelNormal"_s,
                "LevelCriticalNotCharging"_s + event<criticalLevelCheck> [ isCriticalCharging ] / (setCriticalCharging, sendStateChange) = "LevelCriticalCharging"_s,
                "LevelCriticalNotCharging"_s + event<criticalLevelCheck> [ isShutdown ] / (setShutdown, sendStateChange) = "Shutdown"_s,
                "LevelCriticalCharging"_s + event<criticalLevelCheck> [ isCriticalNotCharging && !isShutdown ] / (setCriticalNotCharging, sendStateChange) = "LevelCriticalNotCharging"_s,
                "LevelCriticalCharging"_s + event<criticalLevelCheck> [ isNormal ] / (setNormal, sendStateChange) = "LevelNormal"_s,
                "LevelCriticalCharging"_s + event<criticalLevelCheck> [ isCriticalNotCharging && isShutdown ] / (setShutdown, sendStateChange) = "Shutdown"_s
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
        sm->process_event(initialLevelCheck{});
    }

    void deinit()
    {
        settings->unregisterValueChange(settings::Battery::batteryCriticalLevel, settings::SettingsScope::Global);
        settings.reset();
        sm.reset();
    }

    void checkBatteryLevel()
    {
        sm->process_event(criticalLevelCheck{});
    }

    void setBatteryCriticalLevel(unsigned int level)
    {
        batteryLevelCritical = level;
        settings->setValue(
            settings::Battery::batteryCriticalLevel, utils::to_string(level), settings::SettingsScope::Global);
        checkBatteryLevel();
    }

} // namespace battery_level_check
