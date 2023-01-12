// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryState.hpp"
#include <boost/sml.hpp>
#include <utility>

namespace
{
    namespace sml = boost::sml;
    using State   = BatteryState::ChargingState;

    namespace events
    {
        struct Check
        {
            const float soc;
            const BatteryState::ChargingState state;
            const float criticalThreshold;
            const float shutdownThreshold;
        };
    } // namespace events

    // guards
    struct isNormal
    {
        bool operator()(const events::Check &e) const
        {
            return e.soc >= e.criticalThreshold;
        }
    } isNormal;

    struct isCriticalNotCharging
    {
        bool operator()(const events::Check &e) const
        {
            return e.soc < e.criticalThreshold && e.state != State::Charging && e.state != State::ChargingDone;
        }
    } isCriticalNotCharging;

    struct isCriticalCharging
    {
        bool operator()(const events::Check &e) const
        {
            return e.soc < e.criticalThreshold && (e.state == State::Charging || e.state == State::ChargingDone);
        }
    } isCriticalCharging;

    struct isShutdown
    {
        bool operator()(const events::Check &e) const
        {
            return e.soc < e.shutdownThreshold;
        }
    } isShutdown;

    // actions
    struct setCriticalNotCharging
    {
        void operator()(BatteryState::NotifyStateChangedCallback &notifier)
        {
            notifier(BatteryState::State::CriticalNotCharging);
        }
    } setCriticalNotCharging;

    struct setCriticalCharging
    {
        void operator()(BatteryState::NotifyStateChangedCallback &notifier)
        {
            notifier(BatteryState::State::CriticalCharging);
        }
    } setCriticalCharging;

    struct setNormal
    {
        void operator()(BatteryState::NotifyStateChangedCallback &notifier)
        {
            notifier(BatteryState::State::Normal);
        }
    } setNormal;

    struct setShutdown
    {
        void operator()(BatteryState::NotifyStateChangedCallback &notifier)
        {
            notifier(BatteryState::State::Shutdown);
        }
    } setShutdown;

    struct StateMachine
    {
        auto operator()() const
        {
            using namespace sml;
            // clang-format off
                return make_transition_table(
                *"InitialCheck"_s + event<events::Check> [ isCriticalNotCharging && !isShutdown ] = "LevelCriticalNotCharging"_s,
                "InitialCheck"_s + event<events::Check> [ isCriticalNotCharging && isShutdown ] = "Shutdown"_s,
                "InitialCheck"_s + event<events::Check> [ isCriticalCharging ] = "LevelCriticalCharging"_s,
                "InitialCheck"_s + event<events::Check> [ isNormal ] = "LevelNormal"_s,

                "LevelNormal"_s + sml::on_entry<_> / setNormal,
                "LevelNormal"_s + event<events::Check> [ isCriticalNotCharging && !isShutdown ] = "LevelCriticalNotCharging"_s,
                "LevelNormal"_s + event<events::Check> [ isCriticalNotCharging && isShutdown ] = "Shutdown"_s,
                "LevelNormal"_s + event<events::Check> [ isCriticalCharging ]  = "LevelCriticalCharging"_s,

                "LevelCriticalNotCharging"_s + sml::on_entry<_> / setCriticalNotCharging,
                "LevelCriticalNotCharging"_s + event<events::Check> [ isNormal ]  = "LevelNormal"_s,
                "LevelCriticalNotCharging"_s + event<events::Check> [ isCriticalCharging ]  = "LevelCriticalCharging"_s,
                "LevelCriticalNotCharging"_s + event<events::Check> [ isShutdown ]  = "Shutdown"_s,

                "LevelCriticalCharging"_s + sml::on_entry<_> / setCriticalCharging,
                "LevelCriticalCharging"_s + event<events::Check> [ isCriticalNotCharging && !isShutdown ]  = "LevelCriticalNotCharging"_s,
                "LevelCriticalCharging"_s + event<events::Check> [ isNormal ]  = "LevelNormal"_s,
                "LevelCriticalCharging"_s + event<events::Check> [ isCriticalNotCharging && isShutdown ]  = "Shutdown"_s,

                "Shutdown"_s + sml::on_entry<_> / setShutdown
                );
            // clang-format on
        }
    };

    std::unique_ptr<sml::sm<StateMachine>> sm;
} // namespace

class BatteryState::Pimpl
{
    friend BatteryState;

  public:
    Pimpl(NotifyStateChangedCallback notifyCallback, Thresholds thresholds)
        : thresholds(thresholds), notifyCallback{std::move(notifyCallback)}
    {}

  private:
    const Thresholds thresholds;
    NotifyStateChangedCallback notifyCallback;
    sml::sm<StateMachine, NotifyStateChangedCallback> sm{notifyCallback};
};

void BatteryState::check(const ChargingState state, const float soc)
{
    pimpl->sm.process_event(events::Check{soc, state, pimpl->thresholds.critical, pimpl->thresholds.shutdown});
}

BatteryState::BatteryState(sys::Service *service, NotifyStateChangedCallback notifyCallback, Thresholds thresholds)
    : pimpl{std::make_shared<Pimpl>(notifyCallback, thresholds)}
{}
