// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SettingsPresenter.hpp"
#include "MeditationCommon.hpp"

#include "models/ChimeInterval.hpp"
#include "models/ChimeVolume.hpp"
#include "models/StartDelay.hpp"

#include <ApplicationCommon.hpp>
#include <common/widgets/list_items/Fraction.hpp>
#include <common/widgets/list_items/Numeric.hpp>
#include <common/LanguageUtils.hpp>
#include <apps-common/InternalModel.hpp>
#include <apps-common/widgets/spinners/Spinners.hpp>

namespace app::list_items
{
    struct StartDelayFormatter
    {
      public:
        StartDelayFormatter() : none(utils::translate("app_bell_meditation_interval_none"))
        {}
        std::string operator()(std::uint8_t value) const
        {
            return value == 0 ? none : std::to_string(value);
        }

      private:
        std::string none;
    };

    using StartDelaySpinner = gui::U8IntegerSpinnerWithFormatter<StartDelayFormatter>;
    class StartDelay : public app::list_items::details::ListItemBase<StartDelaySpinner>
    {
      public:
        StartDelay(spinner_type::range &&range,
                   gui::AbstractSettingsModel<spinner_type::value_type> &model,
                   const std::string &topDescription    = "",
                   const std::string &bottomDescription = "")
            : app::list_items::details::ListItemBase<spinner_type>(
                  std::move(range), model, topDescription, bottomDescription)
        {}

      private:
        void control_bottom_description(const spinner_type::value_type &value) final
        {
            bottomText->setVisible(value != 0);
            bottomText->setRichText(utils::language::getCorrectSecondsNumeralForm(value));
        }
    };

} // namespace app::list_items

namespace app::meditation
{
    using namespace gui;
    SettingsPresenter::SettingsPresenter(models::ChimeInterval &chimeIntervalModel,
                                         models::ChimeVolume &chimeVolumeModel,
                                         models::StartDelay &startDelayModel,
                                         AbstractAudioModel &audioModel)
        : chimeIntervalModel{chimeIntervalModel}, chimeVolumeModel{chimeVolumeModel}, startDelayModel{startDelayModel},
          audioModel{audioModel}
    {

        auto chimeInterval =
            new list_items::Fraction{list_items::Fraction::spinner_type::range{{1, 1}, {1, 2}, {1, 3}, {1, 4}},
                                     chimeIntervalModel,
                                     utils::translate("app_bell_meditation_chime_interval"),
                                     utils::translate("app_bell_meditation_chime_interval_bottom")};

        auto startDelay = new list_items::StartDelay{list_items::StartDelay::spinner_type::range{0, 90, 10},
                                                     startDelayModel,
                                                     utils::translate("app_bell_meditation_start_delay"),
                                                     utils::translate("common_second_lower")};

        auto chimeVolume = new list_items::Numeric{list_items::Numeric::spinner_type::range{1, 10, 1},
                                                   chimeVolumeModel,
                                                   utils::translate("app_bell_meditation_chime_volume")};

        listItemsProvider =
            std::make_shared<BellListItemProvider>(BellListItemProvider::Items{startDelay, chimeInterval, chimeVolume});

        auto playSound = [this]() {
            this->audioModel.play(getMeditationAudioPath(), AbstractAudioModel::PlaybackType::Meditation, {});
        };

        chimeVolume->onEnter = playSound;

        chimeVolume->onExit = [this]() { stopSound(); };

        chimeVolume->set_on_value_change_cb([this, playSound](const auto &val) {
            this->audioModel.setVolume(val, AbstractAudioModel::PlaybackType::Meditation, {});
            if (this->audioModel.hasPlaybackFinished()) {
                playSound();
            }
        });

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

    void SettingsPresenter::stopSound()
    {
        audioModel.stopPlayedByThis({});
    }

    void SettingsPresenter::exitWithoutSave()
    {
        this->stopSound();
        chimeVolumeModel.restoreDefault();
        eraseProviderData();
    }

} // namespace app::meditation
