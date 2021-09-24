// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <SoundFilesModel.hpp>

#include <apps-common/widgets/spinners/Spinners.hpp>
#include <common/models/AbstractSettingsModel.hpp>
#include <common/widgets/BellSideListItemWithCallbacks.hpp>

namespace gui
{
    class SoundFileListItem : public BellSideListItemWithCallbacks
    {
      public:
        SoundFileListItem(AbstractSettingsModel<std::string> &model,
                          app::bell_settings::AlarmSoundType soundType,
                          typename ModelDelegateSpinner<app::bell_settings::AlarmSoundDelegate>::Range range,
                          const std::string &topDescription = {});

      private:
        ModelDelegateSpinner<app::bell_settings::AlarmSoundDelegate> *spinner{};
    };
} // namespace gui
