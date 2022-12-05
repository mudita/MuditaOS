// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/widgets/BellSideListItemWithCallbacks.hpp>
#include <common/data/FrontlightUtils.hpp>
#include <apps-common/InternalModel.hpp>

namespace app::bell_settings
{

    class SettingsListItemProvider : public app::InternalModel<gui::BellSideListItemWithCallbacks *>,
                                     public gui::ListItemProvider
    {
      public:
        /// Val contains currently chosen tone
        using ToneCallback = std::function<void(const UTF8 &val)>;
        /// Val contains currently chosen volume(1-10 range)
        using VolumeCallback      = std::function<void(const uint32_t &val)>;
        using VolumeEnterCallback = ToneCallback;
        using FrontlightCallback      = std::function<void(const frontlight_utils::Brightness &val)>;
        using FrontlightEnterCallback = FrontlightCallback;
        using FrontlightExitCallback  = std::function<void()>;

        std::vector<gui::BellSideListItemWithCallbacks *> getListItems();

        auto requestRecords(uint32_t offset, uint32_t limit) -> void override;

        [[nodiscard]] auto getItem(gui::Order order) -> gui::ListItem * override;

        [[nodiscard]] auto requestRecordsCount() -> unsigned int override;

        [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;

        void clearData();

        std::function<void()> onExit;

        ToneCallback onToneEnter;
        ToneCallback onToneExit;
        ToneCallback onToneChange;

        VolumeEnterCallback onVolumeEnter;
        VolumeCallback onVolumeExit;
        VolumeCallback onVolumeChange;

        FrontlightEnterCallback onFrontlightEnter;
        FrontlightExitCallback onFrontlightExit;
        FrontlightCallback onFrontlightChange;
    };

} // namespace app::bell_settings
