// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "StateController.hpp"
#include "HomeScreenPresenter.hpp"

#include <common/TimeUtils.hpp>
#include <common/models/TimeModel.hpp>

#include <keymap/KeyMap.hpp>

#include <models/BatteryModel.hpp>

#include <boost/sml.hpp>
#include <time/time_conversion.hpp>
#include <time/time_constants.hpp>

#include <random>

/// Uncomment to print state machine debug logs
/// #define DEBUG_STATE_MACHINE 1U

#ifdef DEBUG_STATE_MACHINE
#include "StateControllerLogger.hpp"
#endif

namespace app::home_screen
{
    namespace sml = boost::sml;

    namespace
    {
        namespace Helpers
        {
            auto isDeactivated = [](AbstractAlarmModel &alarmModel) {
                return alarmModel.getAlarmStatus() == alarms::AlarmStatus::Deactivated;
            };
            auto isActivated = [](AbstractAlarmModel &alarmModel) {
                return alarmModel.getAlarmStatus() == alarms::AlarmStatus::Activated;
            };
            auto isSnoozed = [](AbstractAlarmModel &alarmModel) {
                return alarmModel.getAlarmStatus() == alarms::AlarmStatus::Snoozed;
            };
            auto isRinging = [](AbstractAlarmModel &alarmModel) {
                return alarmModel.getAlarmStatus() == alarms::AlarmStatus::Ringing;
            };

            auto switchToMenu          = [](AbstractView &view) { view.switchToMenu(); };
            auto makeAlarmEditable     = [](AbstractView &view) { view.setAlarmEdit(true); };
            auto makeAlarmNonEditable  = [](AbstractView &view) { view.setAlarmEdit(false); };
            auto switchToBatteryStatus = [](AbstractView &view) { view.switchToBatteryStatus(); };
            auto updateBottomStats =
                [](AbstractView &view, AbstractBatteryModel &batteryModel, AbstractTemperatureModel &temperatureModel) {
                    view.setTemperature(temperatureModel.getTemperature());
                    view.setBatteryLevelState(batteryModel.getLevelState());
                };
            auto setNewAlarmTime = [](AbstractView &view,
                                      AbstractAlarmModel &alarmModel,
                                      AbstractPresenter &presenter) { alarmModel.setAlarmTime(view.getAlarmTime()); };

            auto isAlarmActive   = [](AbstractAlarmModel &alarmModel) -> bool { return alarmModel.isActive(); };
            auto isSnoozeAllowed = [](AbstractAlarmModel &alarmModel, AbstractController &controller) -> bool {
                return alarmModel.isSnoozeAllowed() && controller.isSnoozeAllowed();
            };
            auto isSnoozeActive = [](AbstractAlarmModel &alarmModel) -> bool { return alarmModel.isSnoozeActive(); };
            auto snooze         = [](AbstractController &controller) { controller.snooze(true); };
            auto getRand        = [](const std::uint32_t lo, const std::uint32_t hi) -> std::uint32_t {
                std::random_device dev;
                std::mt19937 rng(dev());
                std::uniform_int_distribution<std::mt19937::result_type> dist(lo, hi);
                return dist(rng);
            };
            auto getGreeting = []() -> UTF8 {
                const auto greetingCollection = utils::translate_array("app_bell_greeting_msg");
                if (greetingCollection.empty()) {
                    LOG_WARN("app_bell_greeting_msg array does not exist, using default string");
                    return "app_bell_greeting_msg";
                }
                return greetingCollection[Helpers::getRand(0, greetingCollection.size() - 1)];
            };
            auto setDefaultAlarmTime = [](AbstractAlarmModel &alarmModel, AbstractView &view) {
                alarmModel.turnOff();
                alarmModel.setDefaultAlarmTime();
                view.setAlarmTime(alarmModel.getAlarmTime());
            };
            auto turnOffRingingAlarm = [](AbstractAlarmModel &alarmModel) { alarmModel.turnOff(); };
        } // namespace Helpers

        namespace Events
        {
            struct BackPress
            {};
            struct LongBackPress
            {};
            struct LightPress
            {};
            struct RotateRightPress
            {};
            struct RotateLeftPress
            {};
            struct DeepUpPress
            {};
            struct DeepDownPress
            {};
            struct AlarmInput
            {};
            struct Timer
            {};
            struct TimeUpdate
            {};
            struct AlarmRinging
            {};
            struct ModelReady
            {};
            struct Reset
            {};
        } // namespace Events

        namespace Init
        {
            auto entry = [](AbstractView &view,
                            AbstractBatteryModel &batteryModel,
                            AbstractTemperatureModel &temperatureModel,
                            AbstractPresenter &presenter,
                            AbstractAlarmModel &alarmModel) {
                alarmModel.update([&]() { presenter.handleAlarmModelReady(); });
                view.setAlarmEdit(false);
                view.setAlarmActive(false);
                view.setHeaderViewMode(HeaderViewMode::Empty);
                view.setTemperature(temperatureModel.getTemperature());
                view.setBatteryLevelState(batteryModel.getLevelState());
            };
        } // namespace Init

        namespace Deactivated
        {
            auto entry = [](AbstractController &controller,
                            AbstractView &view,
                            AbstractTemperatureModel &temperatureModel,
                            AbstractAlarmModel &alarmModel,
                            AbstractTimeModel &timeModel) {
                controller.snooze(false);
                view.setAlarmEdit(false);
                view.setAlarmActive(false);
                view.setHeaderViewMode(HeaderViewMode::Empty);
                view.setTemperature(temperatureModel.getTemperature());
            };
        } // namespace Deactivated

        namespace DeactivatedWait
        {
            auto entry = [](AbstractView &view, AbstractPresenter &presenter, AbstractAlarmModel &alarmModel) {
                alarmModel.activate(false);
                presenter.spawnTimer();
                view.setBottomDescription(utils::translate("app_bell_alarm_deactivated"));
                view.setAlarmActive(false);
                view.setHeaderViewMode(HeaderViewMode::AlarmIcon);
            };
            auto exit = [](AbstractPresenter &presenter) { presenter.detachTimer(); };
        } // namespace DeactivatedWait

        namespace AlarmEdit
        {
            auto entry = [](AbstractView &view, AbstractPresenter &presenter) {
                view.setAlarmEdit(true);
                view.setHeaderViewMode(HeaderViewMode::AlarmIconAndTime);
            };
            auto exit = [](AbstractView &view, AbstractPresenter &presenter) {
                view.setAlarmEdit(false);
                presenter.detachTimer();
            };

            auto processRotateLeft = [](AbstractView &view, AbstractPresenter &presenter) {
                presenter.spawnTimer(defaultAlarmSetTime);
                view.decAlarmMinute();
            };

            auto processRotateRight = [](AbstractView &view, AbstractPresenter &presenter) {
                presenter.spawnTimer(defaultAlarmSetTime);
                view.incAlarmMinute();
            };
        } // namespace AlarmEdit

        namespace WaitForConfirmation
        {
            auto entry = [](AbstractView &view, AbstractPresenter &presenter) {
                presenter.spawnTimer();
                view.setBottomDescription(utils::translate("app_bellmain_home_screen_bottom_desc_dp"));
            };
            auto exit = [](AbstractPresenter &presenter) { presenter.detachTimer(); };

            auto action = [](AbstractView &view) {
                view.setAlarmEdit(false);
                view.setAlarmActive(true);
            };
        } // namespace WaitForConfirmation

        namespace ActivatedWait
        {
            auto entry = [](AbstractView &view,
                            AbstractPresenter &presenter,
                            AbstractAlarmModel &alarmModel,
                            AbstractTimeModel &timeModel) {
                alarmModel.activate(true);
                presenter.spawnTimer();
                view.setBottomDescription(utils::time::getBottomDescription(
                    utils::time::calculateMinutesDifference(view.getAlarmTime(), timeModel.getCurrentTime())));
                view.setAlarmActive(true);
                view.setHeaderViewMode(HeaderViewMode::AlarmIconAndTime);
            };
            auto exit = [](AbstractPresenter &presenter) { presenter.detachTimer(); };
        } // namespace ActivatedWait

        namespace Activated
        {
            auto entry = [](AbstractController &controller,
                            AbstractView &view,
                            AbstractAlarmModel &alarmModel,
                            AbstractTemperatureModel &temperatureModel) {
                controller.snooze(false);
                view.setTemperature(temperatureModel.getTemperature());
                view.setAlarmActive(true);
                view.setHeaderViewMode(HeaderViewMode::AlarmIconAndTime);
                view.setAlarmTime(alarmModel.getAlarmTime());
            };
        } // namespace Activated

        namespace AlarmRinging
        {
            auto entry =
                [](AbstractView &view, AbstractTemperatureModel &temperatureModel, AbstractPresenter &presenter) {
                    presenter.spawnTimer(defaultAlarmRingingTime);
                    view.setHeaderViewMode(HeaderViewMode::AlarmIcon);
                    view.setAlarmTriggered();
                    view.setTemperature(temperatureModel.getTemperature());
                };
            auto exit = [](AbstractPresenter &presenter) { presenter.detachTimer(); };
        } // namespace AlarmRinging

        namespace AlarmRingingDeactivatedWait
        {
            auto entry = [](AbstractView &view, AbstractAlarmModel &alarmModel, AbstractPresenter &presenter) {
                presenter.spawnTimer();
                alarmModel.turnOff();
                alarmModel.activate(false);
                view.setHeaderViewMode(HeaderViewMode::AlarmIcon);
                view.setBottomDescription(Helpers::getGreeting());
                view.setAlarmActive(false);
            };
            auto exit = [](AbstractPresenter &presenter) { presenter.detachTimer(); };
        } // namespace AlarmRingingDeactivatedWait

        namespace AlarmSnoozedWait
        {
            auto entry = [](AbstractView &view, AbstractAlarmModel &alarmModel, AbstractPresenter &presenter) {
                presenter.spawnTimer();
                alarmModel.snooze();
                view.setHeaderViewMode(HeaderViewMode::SnoozeCountdown);

                const auto snoozeDuration = alarmModel.getTimeToNextSnooze();
                view.setSnoozeTime(alarmModel.getTimeOfNextSnooze());
                const auto bottomDescription = utils::translate("app_bellmain_home_screen_bottom_desc") + " " +
                                               std::to_string(alarmModel.getSnoozeDuration()) + " min";
                view.setBottomDescription(bottomDescription);
            };
            auto exit = [](AbstractPresenter &presenter) { presenter.detachTimer(); };
        } // namespace AlarmSnoozedWait

        namespace AlarmSnoozed
        {
            auto entry =
                [](AbstractView &view, AbstractPresenter &presenter, AbstractTemperatureModel &temperatureModel) {
                    view.setHeaderViewMode(HeaderViewMode::SnoozeCountdown);
                    view.setTemperature(temperatureModel.getTemperature());
                };
            auto exit = [](AbstractPresenter &presenter) { presenter.stopSnoozeTimer(); };
        } // namespace AlarmSnoozed

        class StateMachine
        {
          public:
            auto operator()() const
            {
                using namespace sml;
                // clang-format off
                return make_transition_table(*"Init"_s + sml::on_entry<_> / Init::entry,
                                             "Init"_s + event<Events::ModelReady> [Helpers::isDeactivated] = "Deactivated"_s,
                                             "Init"_s + event<Events::ModelReady> [Helpers::isActivated] = "Activated"_s,
                                             "Init"_s + event<Events::ModelReady> [Helpers::isSnoozed] = "AlarmSnoozed"_s,
                                             "Init"_s + event<Events::ModelReady> [Helpers::isRinging] = "AlarmRinging"_s,

                                             "Deactivated"_s + sml::on_entry<_> / Deactivated::entry,
                                             "Deactivated"_s + event<Events::Reset> = "Init"_s,
                                             "Deactivated"_s + event<Events::LightPress>/ Helpers::switchToMenu,
                                             "Deactivated"_s + event<Events::RotateLeftPress> / Helpers::makeAlarmEditable = "DeactivatedEdit"_s,
                                             "Deactivated"_s + event<Events::RotateRightPress> / Helpers::makeAlarmEditable = "DeactivatedEdit"_s,
                                             "Deactivated"_s + event<Events::DeepUpPress> = "ActivatedWait"_s,
                                             "Deactivated"_s + event<Events::TimeUpdate> / Helpers::updateBottomStats,
                                             "Deactivated"_s + event<Events::LongBackPress>  / Helpers::switchToBatteryStatus,

                                             "DeactivatedWait"_s + sml::on_entry<_> / DeactivatedWait::entry,
                                             "DeactivatedWait"_s + sml::on_exit<_> / DeactivatedWait::exit,
                                             "DeactivatedWait"_s + event<Events::Reset> = "Init"_s,
                                             "DeactivatedWait"_s + event<Events::Timer> = "Deactivated"_s,
                                             "DeactivatedWait"_s + event<Events::LightPress>/ Helpers::switchToMenu = "Deactivated"_s,
                                             "DeactivatedWait"_s + event<Events::DeepUpPress> = "ActivatedWait"_s,
                                             "DeactivatedWait"_s + event<Events::RotateLeftPress> = "DeactivatedEdit"_s,
                                             "DeactivatedWait"_s + event<Events::RotateRightPress> = "DeactivatedEdit"_s,
                                             "DeactivatedWait"_s + event<Events::BackPress> = "Deactivated"_s,

                                             "DeactivatedEdit"_s + sml::on_entry<_> / AlarmEdit::entry,
                                             "DeactivatedEdit"_s + sml::on_exit<_> / AlarmEdit::exit,
                                             "DeactivatedEdit"_s + event<Events::Reset> = "Init"_s,
                                             "DeactivatedEdit"_s + event<Events::TimeUpdate> / Helpers::updateBottomStats,
                                             "DeactivatedEdit"_s + event<Events::Timer> = "Deactivated"_s,
                                             "DeactivatedEdit"_s + event<Events::RotateLeftPress> / AlarmEdit::processRotateLeft,
                                             "DeactivatedEdit"_s + event<Events::RotateRightPress> / AlarmEdit::processRotateRight,
                                             "DeactivatedEdit"_s + event<Events::DeepUpPress> / Helpers::setNewAlarmTime = "ActivatedWait"_s,
                                             "DeactivatedEdit"_s + event<Events::LightPress> / Helpers::setNewAlarmTime = "WaitForConfirmation"_s,
                                             "DeactivatedEdit"_s + event<Events::BackPress> = "Deactivated"_s,

                                             "WaitForConfirmation"_s + sml::on_entry<_> / WaitForConfirmation::entry,
                                             "WaitForConfirmation"_s + sml::on_exit<_> / WaitForConfirmation::exit,
                                             "WaitForConfirmation"_s + event<Events::Reset> = "Init"_s,
                                             "WaitForConfirmation"_s + event<Events::Timer> / Helpers::makeAlarmNonEditable = "Deactivated"_s,
                                             "WaitForConfirmation"_s + event<Events::DeepUpPress> / WaitForConfirmation::action = "ActivatedWait"_s,
                                             "WaitForConfirmation"_s + event<Events::LightPress>/ Helpers::switchToMenu = "Deactivated"_s,
                                             "WaitForConfirmation"_s + event<Events::BackPress> = "Deactivated"_s,
                                             "WaitForConfirmation"_s + event<Events::RotateLeftPress> = "DeactivatedEdit"_s,
                                             "WaitForConfirmation"_s + event<Events::RotateRightPress> = "DeactivatedEdit"_s,

                                             "ActivatedWait"_s + sml::on_entry<_> / ActivatedWait::entry,
                                             "ActivatedWait"_s + sml::on_exit<_> / ActivatedWait::exit,
                                             "ActivatedWait"_s + event<Events::Reset> = "Init"_s,
                                             "ActivatedWait"_s + event<Events::Timer> / Helpers::makeAlarmNonEditable = "Activated"_s,
                                             "ActivatedWait"_s + event<Events::LightPress>/ Helpers::switchToMenu = "Activated"_s,
                                             "ActivatedWait"_s + event<Events::DeepDownPress> = "DeactivatedWait"_s,
                                             "ActivatedWait"_s + event<Events::BackPress> = "Activated"_s,
                                             "ActivatedWait"_s + event<Events::RotateLeftPress> = "ActivatedEdit"_s,
                                             "ActivatedWait"_s + event<Events::RotateRightPress> = "ActivatedEdit"_s,
                                             "ActivatedWait"_s + event<Events::AlarmRinging>  = "AlarmRinging"_s,

                                             "Activated"_s + sml::on_entry<_> / Activated::entry,
                                             "Activated"_s + event<Events::Reset> = "Init"_s,
                                             "Activated"_s [not Helpers::isAlarmActive] = "Deactivated"_s,
                                             "Activated"_s + event<Events::LightPress>/ Helpers::switchToMenu = "Activated"_s,
                                             "Activated"_s + event<Events::RotateLeftPress> / Helpers::makeAlarmEditable = "ActivatedEdit"_s,
                                             "Activated"_s + event<Events::RotateRightPress> / Helpers::makeAlarmEditable = "ActivatedEdit"_s,
                                             "Activated"_s + event<Events::TimeUpdate> / Helpers::updateBottomStats,
                                             "Activated"_s + event<Events::DeepDownPress>  = "DeactivatedWait"_s,
                                             "Activated"_s + event<Events::AlarmRinging>  = "AlarmRinging"_s,
                                             "Activated"_s + event<Events::LongBackPress>  / Helpers::switchToBatteryStatus,

                                             "ActivatedEdit"_s + sml::on_entry<_> / AlarmEdit::entry,
                                             "ActivatedEdit"_s + sml::on_exit<_> / AlarmEdit::exit,
                                             "ActivatedEdit"_s + event<Events::Reset> = "Init"_s,
                                             "ActivatedEdit"_s + event<Events::TimeUpdate> / Helpers::updateBottomStats,
                                             "ActivatedEdit"_s + event<Events::Timer> = "Activated"_s,
                                             "ActivatedEdit"_s + event<Events::RotateLeftPress> / AlarmEdit::processRotateLeft,
                                             "ActivatedEdit"_s + event<Events::RotateRightPress> / AlarmEdit::processRotateRight,
                                             "ActivatedEdit"_s + event<Events::LightPress> / Helpers::setNewAlarmTime = "ActivatedWait"_s,
                                             "ActivatedEdit"_s + event<Events::BackPress> = "Activated"_s,
                                             "ActivatedEdit"_s + event<Events::DeepDownPress> = "DeactivatedWait"_s,

                                             "AlarmRinging"_s + sml::on_entry<_> / AlarmRinging::entry,
                                             "AlarmRinging"_s + sml::on_exit<_> / AlarmRinging::exit,
                                             "AlarmRinging"_s + event<Events::Reset> = "Init"_s,
                                             "AlarmRinging"_s + event<Events::Timer> [Helpers::isSnoozeAllowed] / Helpers::snooze = "AlarmSnoozedWait"_s,
                                             "AlarmRinging"_s + event<Events::Timer> [not Helpers::isSnoozeAllowed] / Helpers::turnOffRingingAlarm = "ActivatedWait"_s,
                                             "AlarmRinging"_s + event<Events::LightPress> [Helpers::isSnoozeAllowed] = "AlarmSnoozedWait"_s,
                                             "AlarmRinging"_s + event<Events::LightPress> [not Helpers::isSnoozeAllowed] / Helpers::turnOffRingingAlarm  = "ActivatedWait"_s,
                                             "AlarmRinging"_s + event<Events::RotateLeftPress> [Helpers::isSnoozeAllowed] = "AlarmSnoozedWait"_s,
                                             "AlarmRinging"_s + event<Events::RotateLeftPress> [not Helpers::isSnoozeAllowed] / Helpers::turnOffRingingAlarm  = "ActivatedWait"_s,
                                             "AlarmRinging"_s + event<Events::RotateRightPress> [Helpers::isSnoozeAllowed] = "AlarmSnoozedWait"_s,
                                             "AlarmRinging"_s + event<Events::RotateRightPress> [not Helpers::isSnoozeAllowed] / Helpers::turnOffRingingAlarm  = "ActivatedWait"_s,
                                             "AlarmRinging"_s + event<Events::BackPress> [Helpers::isSnoozeAllowed] = "AlarmSnoozedWait"_s,
                                             "AlarmRinging"_s + event<Events::BackPress> [not Helpers::isSnoozeAllowed] / Helpers::turnOffRingingAlarm  = "ActivatedWait"_s,
                                             "AlarmRinging"_s + event<Events::DeepDownPress> = "AlarmRingingDeactivatedWait"_s,

                                             "AlarmRingingDeactivatedWait"_s + sml::on_entry<_> / AlarmRingingDeactivatedWait::entry,
                                             "AlarmRingingDeactivatedWait"_s + sml::on_exit<_> / AlarmRingingDeactivatedWait::exit,
                                             "AlarmRingingDeactivatedWait"_s + event<Events::Reset> = "Init"_s,
                                             "AlarmRingingDeactivatedWait"_s + event<Events::Timer> = "Deactivated"_s,
                                             "AlarmRingingDeactivatedWait"_s + event<Events::DeepUpPress> = "ActivatedWait"_s,
                                             "AlarmRingingDeactivatedWait"_s + event<Events::BackPress> = "Deactivated"_s,
                                             "AlarmRingingDeactivatedWait"_s + event<Events::LightPress>/ Helpers::switchToMenu = "Deactivated"_s,
                                             "AlarmRingingDeactivatedWait"_s + event<Events::RotateLeftPress> = "DeactivatedEdit"_s,
                                             "AlarmRingingDeactivatedWait"_s + event<Events::RotateRightPress> = "DeactivatedEdit"_s,

                                             "AlarmSnoozedWait"_s + sml::on_entry<_> / AlarmSnoozedWait::entry,
                                             "AlarmSnoozedWait"_s + sml::on_exit<_> / AlarmSnoozedWait::exit,
                                             "AlarmSnoozedWait"_s + event<Events::Reset> = "Init"_s,
                                             "AlarmSnoozedWait"_s + event<Events::Timer> = "AlarmSnoozed"_s,
                                             "AlarmSnoozedWait"_s + event<Events::DeepDownPress> = "DeactivatedWait"_s,
                                             "AlarmSnoozedWait"_s + event<Events::LightPress>/ Helpers::switchToMenu = "AlarmSnoozed"_s,
                                             "AlarmSnoozedWait"_s + event<Events::BackPress> = "AlarmSnoozed"_s,

                                             "AlarmSnoozed"_s + sml::on_entry<_> / AlarmSnoozed::entry,
                                             "AlarmSnoozed"_s + sml::on_entry<_> / AlarmSnoozed::exit,
                                             "AlarmSnoozed"_s + event<Events::Reset> = "Init"_s,
                                             "AlarmSnoozed"_s + event<Events::ModelReady> [not Helpers::isSnoozeActive] = "Activated"_s,
                                             "AlarmSnoozed"_s + event<Events::AlarmRinging>  = "AlarmRinging"_s,
                                             "AlarmSnoozed"_s + event<Events::DeepDownPress> = "DeactivatedWait"_s,
                                             "AlarmSnoozed"_s + event<Events::LightPress>/Helpers::switchToMenu,
                                             "AlarmSnoozed"_s + event<Events::TimeUpdate> / Helpers::updateBottomStats
                    );
                // clang-format on
            }
        }; // namespace Helpers
    }      // namespace

    class StateController::Impl
    {
      public:
        Impl(AbstractController &controller,
             AbstractView &view,
             AbstractPresenter &presenter,
             AbstractBatteryModel &batteryModel,
             AbstractTemperatureModel &temperatureModel,
             AbstractAlarmModel &alarmModel,
             AbstractTimeModel &timeModel)
            : sm{
#ifdef DEBUG_STATE_MACHINE
                  smLogger,
#endif
                  controller,
                  view,
                  presenter,
                  batteryModel,
                  temperatureModel,
                  alarmModel,
                  timeModel}
        {}
#ifdef DEBUG_STATE_MACHINE
        using SM = sml::sm<StateMachine, sml::logger<Logger>>;
        Logger smLogger;
#else
        using SM = sml::sm<StateMachine>;
#endif
        SM sm;
        std::uint32_t snoozeCount;
        static constexpr auto maxSnoozeCount = 3U;
    };

    StateController::StateController(AbstractView &view,
                                     AbstractPresenter &presenter,
                                     AbstractBatteryModel &batteryModel,
                                     AbstractTemperatureModel &temperatureModel,
                                     AbstractAlarmModel &alarmModel,
                                     AbstractTimeModel &timeModel)
        : pimpl{std::make_unique<StateController::Impl>(
              *this, view, presenter, batteryModel, temperatureModel, alarmModel, timeModel)},
          presenter{presenter}
    {}

    StateController::~StateController() = default;

    bool StateController::handleInputEvent(const gui::InputEvent &inputEvent)
    {
        using namespace sml;
        const auto key = mapKey(inputEvent.getKeyCode());
        switch (key) {
        case KeyMap::Back:
            if (inputEvent.getState() == gui::InputEvent::State::keyReleasedLong) {
                pimpl->sm.process_event(Events::LongBackPress{});
            }
            else {
                pimpl->sm.process_event(Events::BackPress{});
            }
            break;
        case KeyMap::LightPress:
            pimpl->sm.process_event(Events::LightPress{});
            break;
        case KeyMap::RotateRight:
            pimpl->sm.process_event(Events::RotateRightPress{});
            break;
        case KeyMap::RotateLeft:
            pimpl->sm.process_event(Events::RotateLeftPress{});
            break;
        case KeyMap::DeepPressUp:
            pimpl->sm.process_event(Events::DeepUpPress{});
            break;
        case KeyMap::DeepPressDown:
            pimpl->sm.process_event(Events::DeepDownPress{});
            break;
        default:
            break;
        }

        return true;
    }

    bool StateController::handleTimerEvent()
    {
        pimpl->sm.process_event(Events::Timer{});
        presenter.refreshWindow();
        return true;
    }

    bool StateController::handleTimeUpdateEvent()
    {
        pimpl->sm.process_event(Events::TimeUpdate{});
        return true;
    }

    bool StateController::handleAlarmRingingEvent()
    {
        pimpl->sm.process_event(Events::AlarmRinging{});
        return true;
    }

    bool StateController::handleAlarmModelReady()
    {
        pimpl->sm.process_event(Events::ModelReady{});
        return true;
    }
    bool StateController::isSnoozeAllowed()
    {
        return pimpl->snoozeCount < pimpl->maxSnoozeCount;
    }
    void StateController::snooze(bool ctrl)
    {
        if (ctrl) {
            pimpl->snoozeCount++;
        }
        else {
            pimpl->snoozeCount = 0;
        }
    }

    void StateController::resetStateMachine()
    {
        pimpl->sm.process_event(Events::Reset{});
    }

} // namespace app::home_screen
