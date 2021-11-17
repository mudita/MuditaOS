// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "models/alarm_settings/BedtimeSettingsListItemProvider.hpp"
#include <common/models/AbstractAudioModel.hpp>
#include <common/SoundsRepository.hpp>
#include <common/models/AbstractBedtimeModel.hpp>
#include <apps-common/BasePresenter.hpp>
#include <memory>

namespace gui
{
    class ListItemProvider;
} // namespace gui

namespace app::bell_settings
{
    class BedtimeSettingsWindowContract
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
            virtual void exitWithoutSave()                                                  = 0;
        };
    };

    class BedtimeSettingsPresenter : public BedtimeSettingsWindowContract::Presenter
    {
      public:
        BedtimeSettingsPresenter(std::shared_ptr<BedtimeSettingsListItemProvider> provider,
                                 std::shared_ptr<AbstractBedtimeModel> model,
                                 AbstractAudioModel &audioModel,
                                 std::unique_ptr<AbstractSoundsRepository> soundsRepository);

        auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> override;
        auto saveData() -> void override;
        auto loadData() -> void override;
        auto eraseProviderData() -> void override;
        void exitWithoutSave() override;

      private:
        void stopSound();

        std::shared_ptr<BedtimeSettingsListItemProvider> provider;
        std::shared_ptr<AbstractBedtimeModel> model;
        AbstractAudioModel &audioModel;
        std::unique_ptr<AbstractSoundsRepository> soundsRepository;
        UTF8 currentSoundPath;
    };
} // namespace app::bell_settings
