// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <apps-common/AudioOperations.hpp>
#include <common/models/AbstractAudioModel.hpp>
#include <common/models/FrontlightModel.hpp>
#include <common/SoundsRepository.hpp>
#include <memory>

namespace gui
{
    class ListItemProvider;
} // namespace gui

namespace app::bell_settings
{
    class AbstractAlarmSettingsModel;
    class AlarmSettingsListItemProvider;

    class AlarmSettingsWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;
            virtual void exit()      = 0;
        };

        class Presenter : public BasePresenter<View>
        {
          public:
            virtual ~Presenter() noexcept                                                   = default;
            virtual auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> = 0;
            virtual auto loadData() -> void                                                 = 0;
            virtual auto saveData() -> void                                                 = 0;
            virtual auto eraseProviderData() -> void                                        = 0;
            virtual auto exitWithSave() -> void                                             = 0;
            virtual auto exitWithRollback() -> void                                         = 0;
        };
    };

    class AlarmSettingsPresenter : public AlarmSettingsWindowContract::Presenter
    {
      public:
        AlarmSettingsPresenter(std::unique_ptr<AlarmSettingsListItemProvider> &&provider,
                               std::unique_ptr<AbstractAlarmSettingsModel> &&settingsModel,
                               AbstractAudioModel &audioModel,
                               std::unique_ptr<AbstractSoundsRepository> &&soundsRepository,
                               std::unique_ptr<AbstractFrontlightModel> &&frontlight);

        auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> override;
        auto loadData() -> void override;
        auto saveData() -> void;
        auto eraseProviderData() -> void override;
        auto exitWithSave() -> void override;
        auto exitWithRollback() -> void override;

      private:
        auto stopSound() -> void;

        std::shared_ptr<AlarmSettingsListItemProvider> provider;
        std::unique_ptr<AbstractAlarmSettingsModel> settingsModel;
        AbstractAudioModel &audioModel;
        std::unique_ptr<AbstractSoundsRepository> soundsRepository;
        std::unique_ptr<AbstractFrontlightModel> frontlight;
    };
} // namespace app::bell_settings
