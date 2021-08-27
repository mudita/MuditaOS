// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "StateController.hpp"
#include "HomeScreenPresenter.hpp"
#include "KeyMap.hpp"
#include "models/AlarmModel.hpp"
#include "models/TimeModel.hpp"

#include <boost/sml.hpp>
#include <time/time_conversion.hpp>

namespace app::home_screen
{
    namespace sml = boost::sml;

    namespace
    {
        namespace Helpers
        {
            auto spawnTimer           = [](AbstractPresenter &presenter) { presenter.spawnTimer(); };
            auto detachTimer          = [](AbstractPresenter &presenter) { presenter.detachTimer(); };
            auto switchToMenu         = [](AbstractView &view) { view.switchToMenu(); };
            auto makeAlarmEditable    = [](AbstractView &view) { view.setAlarmEdit(true); };
            auto makeAlarmNonEditable = [](AbstractView &view) { view.setAlarmEdit(false); };
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
            struct AlarmInput
            {};
            struct Timer
            {};
            struct TimeUpdate
            {};
        } // namespace Events

        namespace Deactivated
        {
            auto entry = [](AbstractView &view, AbstractTemperatureModel &temperatureModel) {
                view.setAlarmEdit(false);
                view.setAlarmActive(false);
                view.setTemperature(temperatureModel.getTemperature());
            };
        } // namespace Deactivated

        namespace AlarmEdit
        {
            auto entry = [](AbstractView &view, AbstractPresenter &presenter) {
                presenter.spawnTimer();
                view.setAlarmEdit(true);
            };
            auto exit = [](AbstractView &view, AbstractPresenter &presenter) { view.setAlarmEdit(false); };

            auto processRotateLeft = [](AbstractView &view, AbstractPresenter &presenter) {
                presenter.spawnTimer();
                view.decAlarmMinute();
            };

            auto processRotateRight = [](AbstractView &view, AbstractPresenter &presenter) {
                presenter.spawnTimer();
                view.incAlarmMinute();
            };
        }; // namespace AlarmEdit

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
                presenter.spawnTimer();
                view.setBottomDescription(
                    Helpers::setBottomDescription(Helpers::calculateTimeDifference(view, timeModel)));
                view.setAlarmActive(true);
            };
            auto exit = [](AbstractPresenter &presenter) { presenter.detachTimer(); };
        } // namespace ActivatedWait

        namespace Activated
        {
            auto entry = [](AbstractView &view, AbstractTemperatureModel &temperatureModel) {
                view.setTemperature(temperatureModel.getTemperature());
                view.setAlarmActive(true);
            };
        } // namespace Activated

        class StateMachine
        {
          public:
            auto operator()() const
            {
                using namespace sml;
                // clang-format off
                return make_transition_table(*"Deactivated"_s + event<Events::LightPress>/ Helpers::switchToMenu = "Deactivated"_s,
                                             "Deactivated"_s + sml::on_entry<_> / Deactivated::entry,
                                             "Deactivated"_s + event<Events::RotateRightPress> / Helpers::makeAlarmEditable = "DeactivatedEdit"_s,
                                             "Deactivated"_s + event<Events::TimeUpdate> / Helpers::updateTemperature,

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

                                             "Activated"_s + sml::on_entry<_> / Activated::entry,
                                             "Activated"_s + event<Events::LightPress>/ Helpers::switchToMenu = "Activated"_s,
                                             "Activated"_s + event<Events::RotateRightPress> / Helpers::makeAlarmEditable = "ActivatedEdit"_s,
                                             "Activated"_s + event<Events::TimeUpdate> / Helpers::updateTemperature,

                                             "ActivatedEdit"_s + sml::on_entry<_> / AlarmEdit::entry,
                                             "ActivatedEdit"_s + sml::on_exit<_> / AlarmEdit::exit,
                                             "ActivatedEdit"_s + event<Events::TimeUpdate> / Helpers::updateTemperature,
                                             "ActivatedEdit"_s + event<Events::RotateLeftPress> / AlarmEdit::processRotateLeft,
                                             "ActivatedEdit"_s + event<Events::RotateRightPress> / AlarmEdit::processRotateRight,
                                             "ActivatedEdit"_s + event<Events::Timer>  = "ActivatedWait"_s,
                                             "ActivatedEdit"_s + event<Events::LightPress> / Helpers::detachTimer = "ActivatedWait"_s
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
} // namespace app::home_screen
