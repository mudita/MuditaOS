// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "models/AudioErrorModel.hpp"
#include <apps-common/BasePresenter.hpp>
#include <apps-common/AudioOperations.hpp>
#include <models/alarm_settings/AbstractPrewakeUpSettingsModel.hpp>
#include <common/models/AbstractAudioModel.hpp>
#include <common/models/FrontlightModel.hpp>
#include <memory>

namespace gui
{
    class ListItemProvider;
} // namespace gui

namespace app::bell_settings
{
    class PrewakeUpListItemProvider;

    class PrewakeUpWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept                 = default;
            virtual auto exit() -> void              = 0;
            virtual auto handleError() -> void       = 0;
            virtual auto handleDeletedFile() -> void = 0;
        };

        class Presenter : public BasePresenter<View>
        {
          public:
            virtual ~Presenter() noexcept                                                   = default;
            virtual auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> = 0;
            virtual auto saveData() -> void                                                 = 0;
            virtual auto loadData() -> void                                                 = 0;
            virtual auto eraseProviderData() -> void                                        = 0;
            virtual auto exitWithoutSave() -> void                                          = 0;
        };
    };

    class PrewakeUpWindowPresenter : public PrewakeUpWindowContract::Presenter
    {
      public:
        PrewakeUpWindowPresenter(std::unique_ptr<PrewakeUpListItemProvider> &&provider,
                                 std::unique_ptr<AbstractPrewakeUpSettingsModel> &&model,
                                 AbstractAudioModel &audioModel,
                                 std::unique_ptr<AbstractFrontlightModel> &&frontlight,
                                 std::unique_ptr<AudioErrorModel> &&audioErrorModel);

        auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> override;
        auto saveData() -> void override;
        auto loadData() -> void override;
        auto eraseProviderData() -> void override;
        auto exitWithoutSave() -> void override;

      private:
        auto stopSound() -> void;
        auto handleAudioError(const UTF8 &path, gui::AudioErrorType errorType) -> void;
        auto showAudioError(gui::AudioErrorType errorType) const -> void;
        auto validatePath(const UTF8 &path) const -> bool;

        std::shared_ptr<PrewakeUpListItemProvider> provider;
        std::unique_ptr<AbstractPrewakeUpSettingsModel> model;
        AbstractAudioModel &audioModel;
        std::unique_ptr<AbstractFrontlightModel> frontlight;
        std::unique_ptr<AudioErrorModel> audioErrorModel;
        UTF8 currentSoundPath;
    };
} // namespace app::bell_settings
