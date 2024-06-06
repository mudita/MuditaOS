// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <common/BellListItemProvider.hpp>
#include <common/models/AbstractAudioModel.hpp>
#include <memory>

namespace app
{
    class ApplicationCommon;
}

namespace app::focus
{
    namespace models
    {
        class FocusSettingsModel;
        class NotificationVolumeModel;
    } // namespace models

    class SettingsContract
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
            virtual void loadData()                                                         = 0;
            virtual void saveData()                                                         = 0;
            virtual void eraseProviderData()                                                = 0;
            virtual void exitWithSave()                                                     = 0;
            virtual void exitWithoutSave()                                                  = 0;
        };
    };

    class SettingsPresenter : public SettingsContract::Presenter
    {
      public:
        SettingsPresenter(models::FocusSettingsModel &focusTimeModel,
                          models::FocusSettingsModel &focusRepeatsModel,
                          models::FocusSettingsModel &shortBreakTimeModel,
                          models::FocusSettingsModel &longBreakTimeModel,
                          models::FocusSettingsModel &longBreakOccurrenceModel,
                          models::NotificationVolumeModel &notificationVolumeModel,
                          AbstractAudioModel &audioModel);

        void loadData() override;
        void saveData() override;
        auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> override;
        void eraseProviderData() override;
        void exitWithSave() override;
        void exitWithoutSave() override;

      private:
        void stopSound();

        models::FocusSettingsModel &focusTimeModel;
        models::FocusSettingsModel &focusRepeatsModel;
        models::FocusSettingsModel &shortBreakTimeModel;
        models::FocusSettingsModel &longBreakTimeModel;
        models::FocusSettingsModel &longBreakOccurrenceModel;
        models::NotificationVolumeModel &notificationVolumeModel;
        AbstractAudioModel &audioModel;
        std::shared_ptr<BellListItemProvider> listItemsProvider;
    };
} // namespace app::focus
