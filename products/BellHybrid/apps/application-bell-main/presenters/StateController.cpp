// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "StateController.hpp"
#include "HomeScreenPresenter.hpp"
#include "models/TimeModel.hpp"

#include <keymap/KeyMap.hpp>

#include <boost/sml.hpp>
#include <time/time_conversion.hpp>
#include <time/time_constants.hpp>

namespace app::home_screen
{
    namespace sml = boost::sml;

    namespace
    {
        namespace Helpers
        {
            auto detachTimer          = [](AbstractPresenter &presenter) { presenter.detachTimer(); };
            auto switchToMenu         = [](AbstractView &view) { view.switchToMenu(); };
            auto makeAlarmEditable    = [](AbstractView &view) { view.setAlarmEdit(true); };
            auto makeAlarmNonEditable = [](AbstractView &view) { view.setAlarmEdit(false); };
            auto hideAlarmTime        = [](AbstractView &view) { view.setAlarmTimeVisible(false); };
            auto showAlarmTime        = [](AbstractView &view) { view.setAlarmTimeVisible(true); };
            auto updateTemperature    = [](AbstractView &view, AbstractTemperatureModel &temperatureModel) {
                view.setTemperature(temperatureModel.getTemperature());
            };

            auto calculateTimeDifference = [](AbstractView &view, AbstractTimeModel &timeModel) {
                const auto now       = timeModel.getCurrentTime();
                const auto alarmTime = view.getAlarmTime();
                return utils::time::Duration{alarmTime, now}.get();
            };

            auto setBottomDescription = [](std::time_t timestamp) {
                const auto duration = utils::time::Duration{timestamp};
                const auto prefix   = utils::translate("app_bellmain_home_screen_bottom_desc");
                return UTF8(prefix + " " + std::to_string(duration.getHours()) + " hrs & " +
                            std::to_string(duration.getMinutes()) + " min");
            };

            auto setDefaultAlarmTime = [](AbstractView &view, AbstractTimeModel &timeModel) {
                constexpr auto defaultAlarmTimeHour = 7U;
                constexpr auto defaultAlarmTimeMin  = 0U;
                const auto now                      = timeModel.getCurrentTime();
                const auto newTime                  = std::localtime(&now);
                newTime->tm_hour                    = defaultAlarmTimeHour;
                newTime->tm_min                     = defaultAlarmTimeMin;
                auto alarmTime                      = std::mktime(newTime);

                if (alarmTime < now) {
                    alarmTime += utils::time::secondsInDay;
                }
                view.setAlarmTime(alarmTime);
            };

            auto isAlarmActive = [](AbstractAlarmModel &alarmModel) -> bool { return alarmModel.isActive(); };

        } // namespace Helpers

        namespace Events
        {
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
        } // namespace Events

        namespace Deactivated
        {
            auto entry =
                [](AbstractView &view, AbstractTemperatureModel &temperatureModel, AbstractTimeModel &timeModel) {
                    Helpers::setDefaultAlarmTime(view, timeModel);
                    view.setAlarmEdit(false);
                    view.setAlarmActive(false);
                    view.setAlarmVisible(false);
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
            };
            auto exit = [](AbstractPresenter &presenter) { presenter.detachTimer(); };
        } // namespace DeactivatedWait

        namespace AlarmEdit
        {
            auto entry = [](AbstractView &view, AbstractPresenter &presenter) {
                presenter.spawnTimer();
                view.setAlarmEdit(true);
                view.setAlarmTimeVisible(true);
                view.setAlarmVisible(true);
            };
            auto exit = [](AbstractView &view, AbstractAlarmModel &alarmModel) {
                view.setAlarmEdit(false);
                alarmModel.setAlarmTime(view.getAlarmTime());
            };

            auto processRotateLeft = [](AbstractView &view, AbstractPresenter &presenter) {
                presenter.spawnTimer();
                view.decAlarmMinute();
            };

            auto processRotateRight = [](AbstractView &view, AbstractPresenter &presenter) {
                presenter.spawnTimer();
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
                view.setBottomDescription(
                    Helpers::setBottomDescription(Helpers::calculateTimeDifference(view, timeModel)));
                view.setAlarmActive(true);
                view.setAlarmVisible(true);
            };
            auto exit = [](AbstractPresenter &presenter) { presenter.detachTimer(); };
        } // namespace ActivatedWait

        namespace Activated
        {
            auto entry =
                [](AbstractView &view, AbstractAlarmModel &alarmModel, AbstractTemperatureModel &temperatureModel) {
                    view.setAlarmTime(alarmModel.getAlarmTime());
                    view.setTemperature(temperatureModel.getTemperature());
                    view.setAlarmActive(true);
                    view.setAlarmVisible(true);
                };
        } // namespace Activated

        namespace AlarmRinging
        {
            auto entry = [](AbstractView &view, AbstractPresenter &presenter) {
                presenter.spawnTimer();
                view.setAlarmTimeVisible(false);
                view.setAlarmTriggered();
            };
            auto exit = [](AbstractPresenter &presenter) { presenter.detachTimer(); };
        } // namespace AlarmRinging

        namespace AlarmRingingDeactivatedWait
        {
            auto entry = [](AbstractView &view, AbstractPresenter &presenter) {
                presenter.spawnTimer();
                view.setBottomDescription(utils::translate("app_bell_alarm_ringing_deactivated"));
                view.setAlarmActive(false);
            };
            auto exit = [](AbstractPresenter &presenter) { presenter.detachTimer(); };
        } // namespace AlarmRingingDeactivatedWait

        namespace AlarmSnoozedWait
        {
            auto entry = [](AbstractView &view, AbstractPresenter &presenter) {
                presenter.spawnTimer();
                view.setAlarmTimeVisible(false);
                view.setAlarmSnoozed();
                const auto bottomDescription = utils::translate("app_bellmain_home_screen_bottom_desc") +
                                               " 10 min"; // TODO: Get duration from settings
                view.setBottomDescription(bottomDescription);
            };
            auto exit = [](AbstractPresenter &presenter) { presenter.detachTimer(); };
        } // namespace AlarmSnoozedWait

        namespace AlarmSnoozed
        {
            auto entry =
                [](AbstractView &view, AbstractPresenter &presenter, AbstractTemperatureModel &temperatureModel) {
                    presenter.spawnTimer();
                    view.setAlarmTimeVisible(false);
                    view.setAlarmSnoozed();
                    view.setTemperature(temperatureModel.getTemperature());
                };
            auto exit = [](AbstractPresenter &presenter) { presenter.detachTimer(); };
        } // namespace AlarmSnoozed

        class StateMachine
        {
          public:
            auto operator()() const
            {
                using namespace sml;
                // clang-format off
                return make_transition_table(*"Deactivated"_s + event<Events::LightPress>/ Helpers::switchToMenu = "Deactivated"_s,
                                             "Deactivated"_s + sml::on_entry<_> / Deactivated::entry,
                                             "Deactivated"_s [Helpers::isAlarmActive] = "Activated"_s,
                                             "Deactivated"_s + event<Events::RotateRightPress> / Helpers::makeAlarmEditable = "DeactivatedEdit"_s,
                                             "Deactivated"_s + event<Events::DeepUpPress> / Helpers::showAlarmTime = "ActivatedWait"_s,
                                             "Deactivated"_s + event<Events::TimeUpdate> / Helpers::updateTemperature,

                                             "DeactivatedWait"_s + sml::on_entry<_> / DeactivatedWait::entry,
                                             "DeactivatedWait"_s + sml::on_exit<_> / DeactivatedWait::exit,
                                             "DeactivatedWait"_s + event<Events::Timer> = "Deactivated"_s,
                                             "DeactivatedWait"_s + event<Events::LightPress>/ Helpers::switchToMenu = "Deactivated"_s,

                                             "DeactivatedEdit"_s + sml::on_entry<_> / AlarmEdit::entry,
                                             "DeactivatedEdit"_s + sml::on_exit<_> / AlarmEdit::exit,
                                             "DeactivatedEdit"_s + event<Events::TimeUpdate> / Helpers::updateTemperature,
                                             "DeactivatedEdit"_s + event<Events::RotateLeftPress> / AlarmEdit::processRotateLeft,
                                             "DeactivatedEdit"_s + event<Events::RotateRightPress> / AlarmEdit::processRotateRight,
                                             "DeactivatedEdit"_s + event<Events::DeepUpPress> / Helpers::detachTimer = "ActivatedWait"_s,
                                             "DeactivatedEdit"_s + event<Events::Timer>  = "WaitForConfirmation"_s,
                                             "DeactivatedEdit"_s + event<Events::LightPress> / Helpers::detachTimer = "WaitForConfirmation"_s,

                                             "WaitForConfirmation"_s + sml::on_entry<_> / WaitForConfirmation::entry,
                                             "WaitForConfirmation"_s + sml::on_exit<_> / WaitForConfirmation::exit,
                                             "WaitForConfirmation"_s + event<Events::Timer> / Helpers::makeAlarmNonEditable = "Deactivated"_s,
                                             "WaitForConfirmation"_s + event<Events::DeepUpPress> / WaitForConfirmation::action = "ActivatedWait"_s,

                                             "ActivatedWait"_s + sml::on_entry<_> / ActivatedWait::entry,
                                             "ActivatedWait"_s + sml::on_exit<_> / ActivatedWait::exit,
                                             "ActivatedWait"_s + event<Events::Timer> / Helpers::makeAlarmNonEditable = "Activated"_s,
                                             "ActivatedWait"_s + event<Events::LightPress>/ Helpers::switchToMenu = "Activated"_s,

                                             "Activated"_s + sml::on_entry<_> / Activated::entry,
                                             "Activated"_s + event<Events::LightPress>/ Helpers::switchToMenu = "Activated"_s,
                                             "Activated"_s + event<Events::RotateRightPress> / Helpers::makeAlarmEditable = "ActivatedEdit"_s,
                                             "Activated"_s + event<Events::TimeUpdate> / Helpers::updateTemperature,
                                             "Activated"_s + event<Events::DeepDownPress> / Helpers::hideAlarmTime  = "DeactivatedWait"_s,
                                             "Activated"_s + event<Events::AlarmRinging>  = "AlarmRinging"_s,

                                             "ActivatedEdit"_s + sml::on_entry<_> / AlarmEdit::entry,
                                             "ActivatedEdit"_s + sml::on_exit<_> / AlarmEdit::exit,
                                             "ActivatedEdit"_s + event<Events::TimeUpdate> / Helpers::updateTemperature,
                                             "ActivatedEdit"_s + event<Events::RotateLeftPress> / AlarmEdit::processRotateLeft,
                                             "ActivatedEdit"_s + event<Events::RotateRightPress> / AlarmEdit::processRotateRight,
                                             "ActivatedEdit"_s + event<Events::Timer>  = "ActivatedWait"_s,
                                             "ActivatedEdit"_s + event<Events::LightPress> / Helpers::detachTimer = "ActivatedWait"_s,

                                             "AlarmRinging"_s + sml::on_entry<_> / AlarmRinging::entry,
                                             "AlarmRinging"_s + sml::on_exit<_> / AlarmRinging::exit,
                                             "AlarmRinging"_s + event<Events::Timer> = "AlarmSnoozedWait"_s,
                                             "AlarmRinging"_s + event<Events::LightPress> = "AlarmSnoozedWait"_s,
                                             "AlarmRinging"_s + event<Events::RotateLeftPress> = "AlarmSnoozedWait"_s,
                                             "AlarmRinging"_s + event<Events::RotateRightPress> = "AlarmSnoozedWait"_s,
                                             "AlarmRinging"_s + event<Events::DeepDownPress> = "AlarmRingingDeactivatedWait"_s,

                                             "AlarmRingingDeactivatedWait"_s + sml::on_entry<_> / AlarmRingingDeactivatedWait::entry,
                                             "AlarmRingingDeactivatedWait"_s + sml::on_exit<_> / AlarmRingingDeactivatedWait::exit,
                                             "AlarmRingingDeactivatedWait"_s + event<Events::Timer> = "Deactivated"_s,
                                             "AlarmRingingDeactivatedWait"_s + event<Events::DeepDownPress> = "DeactivatedWait"_s,

                                             "AlarmSnoozedWait"_s + sml::on_entry<_> / AlarmSnoozedWait::entry,
                                             "AlarmSnoozedWait"_s + sml::on_exit<_> / AlarmSnoozedWait::exit,
                                             "AlarmSnoozedWait"_s + event<Events::Timer> = "AlarmSnoozed"_s,
                                             "AlarmSnoozedWait"_s + event<Events::DeepDownPress> = "DeactivatedWait"_s,

                                             "AlarmSnoozed"_s + sml::on_entry<_> / AlarmSnoozed::entry,
                                             "AlarmSnoozed"_s + sml::on_entry<_> / AlarmSnoozed::exit,
                                             "AlarmSnoozed"_s + event<Events::Timer> = "AlarmRinging"_s,
                                             "AlarmSnoozed"_s + event<Events::DeepDownPress> = "DeactivatedWait"_s
                    );
                // clang-format on
            }
        };
    } // namespace

    class StateController::Impl
    {
      public:
        Impl(AbstractView &view,
             AbstractPresenter &presenter,
             AbstractTemperatureModel &temperatureModel,
             AbstractAlarmModel &alarmModel,
             AbstractTimeModel &timeModel)
            : sm{view, presenter, temperatureModel, alarmModel, timeModel}
        {}

        using SM = sml::sm<StateMachine>;
        SM sm;
    };

    StateController::StateController(AbstractView &view,
                                     AbstractPresenter &presenter,
                                     AbstractTemperatureModel &temperatureModel,
                                     AbstractAlarmModel &alarmModel,
                                     AbstractTimeModel &timeModel)
        : pimpl{std::make_unique<StateController::Impl>(view, presenter, temperatureModel, alarmModel, timeModel)},
          presenter{presenter}
    {}

    StateController::~StateController() = default;

    bool StateController::handleInputEvent(const gui::InputEvent &inputEvent)
    {
        using namespace sml;
        const auto key = mapKey(inputEvent.getKeyCode());
        switch (key) {
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
} // namespace app::home_screen
