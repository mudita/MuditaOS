// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FocusSettingsPresenter.hpp"
#include "data/FocusCommon.hpp"

#include "models/FocusSettingsModel.hpp"
#include "models/NotificationVolumeModel.hpp"

#include <ApplicationCommon.hpp>
#include <apps-common/InternalModel.hpp>
#include <common/widgets/list_items/Numeric.hpp>
#include <common/widgets/list_items/NumericWithBar.hpp>
#include <common/widgets/list_items/MinutesWithOff.hpp>
#include <common/widgets/list_items/Multiplicity.hpp>
#include <gui/widgets/ListViewEngine.hpp>

namespace
{
    enum class ListIndex
    {
        FocusTime = 0,
        FocusRepeats,
        ShortBreakTime,
        LongBreakTime,
        LongBreakOccurrence,
        NotificationVolume
    };

    class FocusTimeMultiplicity : public app::list_items::Numeric
    {
      public:
        FocusTimeMultiplicity(spinner_type::range &&range,
                              gui::AbstractSettingsModel<spinner_type::value_type> &model,
                              const std::string &topDescription    = "",
                              const std::string &bottomDescription = "")
            : Numeric{std::move(range), model, topDescription, bottomDescription}
        {}

      private:
        auto getMultiplicityForm(std::uint32_t val) -> std::string
        {
            return (val == 1) ? utils::translate("app_bell_focus_time_once")
                              : utils::translate("app_bell_focus_time_many");
        }

        auto control_bottom_description(const spinner_type::value_type &value) -> void final
        {
            bottomText->setRichText(getMultiplicityForm(value));
        }
    };
} // namespace

namespace app::focus
{
    using namespace gui;
    SettingsPresenter::SettingsPresenter(models::FocusSettingsModel &focusTimeModel,
                                         models::FocusSettingsModel &focusRepeatsModel,
                                         models::FocusSettingsModel &shortBreakTimeModel,
                                         models::FocusSettingsModel &longBreakTimeModel,
                                         models::FocusSettingsModel &longBreakOccurrenceModel,
                                         models::NotificationVolumeModel &notificationVolumeModel,
                                         AbstractAudioModel &audioModel)
        : focusTimeModel{focusTimeModel}, focusRepeatsModel{focusRepeatsModel},
          shortBreakTimeModel{shortBreakTimeModel}, longBreakTimeModel{longBreakTimeModel},
          longBreakOccurrenceModel{longBreakOccurrenceModel}, notificationVolumeModel{notificationVolumeModel},
          audioModel{audioModel}
    {
        auto focusTime = new list_items::Numeric{
            list_items::Numeric::spinner_type::range{
                models::settings::focusTimeMin, models::settings::focusTimeMax, models::settings::focusTimeStep},
            focusTimeModel,
            utils::translate("app_bell_focus_time"),
            utils::translate("common_minutes_lower_genitive")};

        auto focusRepeats = new list_items::Multiplicity{
            list_items::Multiplicity::spinner_type::range{models::settings::focusRepeatsMin,
                                                          models::settings::focusRepeatsMax,
                                                          models::settings::focusRepeatsStep},
            focusRepeatsModel,
            utils::translate("app_bell_focus_repeats"),
            utils::translate("common_multiplicity_many")};

        auto shortBreakTime = new list_items::MinutesWithOff{
            list_items::MinutesWithOff::spinner_type::range{models::settings::shortBreakTimeMin,
                                                            models::settings::shortBreakTimeMax,
                                                            models::settings::shortBreakTimeStep},
            shortBreakTimeModel,
            utils::translate("app_bell_focus_short_break"),
            utils::translate("common_minutes_lower_genitive")};

        auto longBreakTime = new list_items::MinutesWithOff{
            list_items::MinutesWithOff::spinner_type::range{models::settings::longBreakTimeMin,
                                                            models::settings::longBreakTimeMax,
                                                            models::settings::longBreakTimeStep},
            longBreakTimeModel,
            utils::translate("app_bell_focus_long_break"),
            utils::translate("common_minutes_lower_genitive")};

        auto longBreakOccurrence = new FocusTimeMultiplicity{
            FocusTimeMultiplicity::spinner_type::range{models::settings::longBreakOccurrenceMin,
                                                       models::settings::longBreakOccurrenceMax,
                                                       models::settings::longBreakOccurrenceStep},
            longBreakOccurrenceModel,
            utils::translate("app_bell_focus_long_break_after"),
            utils::translate("app_bell_focus_time_many")};

        auto notificationVolume =
            new list_items::NumericWithBar{list_items::NumericWithBar::spinner_type::range{
                                               AbstractAudioModel::minVolume, AbstractAudioModel::maxVolume, 1},
                                           notificationVolumeModel,
                                           AbstractAudioModel::maxVolume,
                                           utils::translate("app_bell_focus_notification_volume")};

        listItemsProvider = std::make_shared<BellListItemProvider>(BellListItemProvider::Items{
            focusTime, focusRepeats, shortBreakTime, longBreakTime, longBreakOccurrence, notificationVolume});

        focusRepeats->onExit = [this, focusRepeats, longBreakOccurrence]() {
            const auto currentValue = longBreakOccurrence->value();
            const std::uint8_t maxValue =
                std::max(models::settings::longBreakOccurrenceMin, focusRepeats->value() - 1U);
            longBreakOccurrence->set_range(list_items::Multiplicity::spinner_type::range{
                models::settings::longBreakOccurrenceMin, maxValue, models::settings::longBreakOccurrenceStep});
            longBreakOccurrence->set_value(std::min(currentValue, maxValue));
        };

        focusRepeats->onProceed = [focusRepeats, this]() {
            if (focusRepeats->value() == 1) {
                // when the user sets only one focus session, the break settings does not matter because we do not
                // display any break after the last focus session
                listItemsProvider->list->rebuildList(gui::listview::RebuildType::OnOffset,
                                                     static_cast<unsigned int>(ListIndex::NotificationVolume));
                return true;
            }
            return false;
        };

        longBreakTime->onProceed = [longBreakTime, this]() {
            if (longBreakTime->value() == 0) {
                listItemsProvider->list->rebuildList(gui::listview::RebuildType::OnOffset,
                                                     static_cast<unsigned int>(ListIndex::NotificationVolume));
                return true;
            }
            return false;
        };

        auto playSound = [this, notificationVolume]() {
            this->audioModel.setVolume(notificationVolume->value(), AbstractAudioModel::PlaybackType::FocusTimer);
            this->audioModel.play(getFocusTimerAudioPath(),
                                  AbstractAudioModel::PlaybackType::FocusTimer,
                                  AbstractAudioModel::PlaybackMode::Single,
                                  {});
        };

        notificationVolume->onEnter = playSound;
        notificationVolume->onExit  = [this]() { stopSound(); };

        notificationVolume->set_on_value_change_cb([this, playSound](const auto &val) {
            this->audioModel.setVolume(
                val, AbstractAudioModel::PlaybackType::FocusTimer, audio::VolumeUpdateType::SkipUpdateDB);
            if (this->audioModel.hasPlaybackFinished()) {
                playSound();
            }
        });

        notificationVolume->onReturn = [this, focusRepeats, longBreakTime]() {
            if (focusRepeats->value() == 1) {
                listItemsProvider->list->rebuildList(gui::listview::RebuildType::OnOffset,
                                                     static_cast<unsigned int>(ListIndex::FocusRepeats));
                return true;
            }
            else if (longBreakTime->value() == 0) {
                listItemsProvider->list->rebuildList(gui::listview::RebuildType::OnOffset,
                                                     static_cast<unsigned int>(ListIndex::LongBreakTime));
                return true;
            }
            return false;
        };

        for (auto &item : listItemsProvider->getListItems()) {
            item->setValue();
        }
    }

    void SettingsPresenter::loadData()
    {
        for (auto &item : listItemsProvider->getListItems()) {
            item->setValue();
        }
    }

    void SettingsPresenter::saveData()
    {
        for (auto &item : listItemsProvider->getListItems()) {
            item->getValue();
        }
    }

    auto SettingsPresenter::getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider>
    {
        return listItemsProvider;
    }

    void SettingsPresenter::eraseProviderData()
    {
        listItemsProvider->clearData();
    }

    void SettingsPresenter::exitWithSave()
    {
        saveData();
        eraseProviderData();
    }

    void SettingsPresenter::exitWithoutSave()
    {
        stopSound();
        notificationVolumeModel.restoreDefault();
        eraseProviderData();
    }

    void SettingsPresenter::stopSound()
    {
        audioModel.stopPlayedByThis({});
    }
} // namespace app::focus
