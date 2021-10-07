// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <models/alarm_settings/AbstractSnoozeSettingsModel.hpp>
#include <common/models/AbstractAudioModel.hpp>
#include <common/SoundsRepository.hpp>
#include <memory>

namespace gui
{
    class ListItemProvider;
} // namespace gui

namespace app::bell_settings
{
    class SnoozeListItemProvider;

    class View
    {
      public:
        virtual ~View() noexcept = default;
        virtual void exit()      = 0;
    };

    class AbstractSnoozePresenter : public BasePresenter<View>
    {
      public:
        virtual ~AbstractSnoozePresenter()                                              = default;
        virtual auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> = 0;
        virtual void saveData()                                                         = 0;
        virtual void loadData()                                                         = 0;
    };

    class SnoozePresenter : public AbstractSnoozePresenter
    {
      public:
        SnoozePresenter(std::shared_ptr<SnoozeListItemProvider> provider,
                        std::unique_ptr<AbstractSnoozeSettingsModel> snoozeSettingsModel,
                        std::unique_ptr<AbstractAudioModel> audioModel,
                        std::unique_ptr<AbstractSoundsRepository> soundsRepository);
        auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> override;
        void saveData() override;
        void loadData() override;

      private:
        void stopSound();

        std::shared_ptr<SnoozeListItemProvider> provider;
        std::unique_ptr<AbstractSnoozeSettingsModel> snoozeSettingsModel;
        std::unique_ptr<AbstractAudioModel> audioModel;
        std::unique_ptr<AbstractSoundsRepository> soundsRepository;

        audio::Token currentToken;
        UTF8 currentSoundPath;
    };
} // namespace app::bell_settings
