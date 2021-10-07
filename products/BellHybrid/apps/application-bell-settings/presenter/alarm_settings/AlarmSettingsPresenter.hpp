// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <apps-common/AudioOperations.hpp>
#include <common/models/AbstractAudioModel.hpp>
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
            virtual auto saveData() -> void                                                 = 0;
            virtual auto loadData() -> void                                                 = 0;
            virtual auto eraseProviderData() -> void                                        = 0;
        };
    };

    class AlarmSettingsPresenter : public AlarmSettingsWindowContract::Presenter
    {
      public:
        AlarmSettingsPresenter(std::shared_ptr<AlarmSettingsListItemProvider> provider,
                               std::unique_ptr<AbstractAlarmSettingsModel> model,
                               std::unique_ptr<AbstractAudioModel> audioModel,
                               std::unique_ptr<AbstractSoundsRepository> soundsRepository);

        auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> override;
        auto saveData() -> void override;
        auto loadData() -> void override;
        auto eraseProviderData() -> void override;

      private:
        void stopSound();

        std::shared_ptr<AlarmSettingsListItemProvider> provider;
        std::unique_ptr<AbstractAlarmSettingsModel> model;
        std::unique_ptr<AbstractAudioModel> audioModel;
        std::unique_ptr<AbstractSoundsRepository> soundsRepository;
        audio::Token currentToken;
    };
} // namespace app::bell_settings
