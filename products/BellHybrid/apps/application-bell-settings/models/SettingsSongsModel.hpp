// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/models/SongsModel.hpp>

namespace app::bell_settings
{
    class SettingsSongsModel : public SongsModel
    {
      public:
        using OnScrollCallback = std::function<bool(const db::multimedia_files::MultimediaFilesRecord &selectedSound)>;

        SettingsSongsModel(ApplicationCommon *application,
                           std::unique_ptr<AbstractSoundsRepository> soundsRepository,
                           const LabelsWithPaths &pathPrefixes = {});

        gui::ListItem *getItem(gui::Order order) override;
        void createData(OnActivateCallback activateCallback, OnScrollCallback scrollCallback);

      private:
        OnScrollCallback scrollCallback{nullptr};
    };
} // namespace app::bell_settings
