// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "models/SettingsSongsModel.hpp"

#include <apps-common/BasePresenter.hpp>
#include <common/models/AbstractAudioModel.hpp>

namespace app::music
{
    class AbstractSongsRepository;
}

namespace app::bell_settings
{
    class BedtimeSettingsWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;

            virtual void updateViewState() = 0;
            virtual void handleError()     = 0;
            virtual bool onSelect()        = 0;
            virtual bool onReturn()        = 0;
        };

        class Presenter : public BasePresenter<BedtimeSettingsWindowContract::View>
        {

          public:
            virtual void createData(SettingsSongsModel::OnActivateCallback activateCallback,
                                    SettingsSongsModel::OnScrollCallback scrollCallback) = 0;
            virtual void updateViewState()                                               = 0;
            virtual void updateRecordsCount()                                            = 0;
            virtual std::shared_ptr<SettingsSongsModel> getSongsModel()                  = 0;
            virtual void playSong(std::string path)                                      = 0;
            virtual void stopSong()                                                      = 0;
        };
    };

    class BedTimeSettingsPresenter : public BedtimeSettingsWindowContract::Presenter
    {
      public:
        BedTimeSettingsPresenter(std::unique_ptr<SettingsSongsModel> songsModel, AbstractAudioModel &audioModel);

        void playSong(std::string path) override;
        void stopSong() override;

      private:
        std::shared_ptr<SettingsSongsModel> songsModel;
        AbstractAudioModel &audioModel;

        void createData(SettingsSongsModel::OnActivateCallback activateCallback,
                        SettingsSongsModel::OnScrollCallback scrollCallback) override;
        void updateViewState() override;
        void updateRecordsCount() override;
        std::shared_ptr<SettingsSongsModel> getSongsModel() override;
    };
} // namespace app::bell_settings
