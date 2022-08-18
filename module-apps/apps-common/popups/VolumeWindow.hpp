// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "apps-common/widgets/BarGraph.hpp"
#include "WindowWithTimer.hpp"

#include <module-audio/Audio/AudioCommon.hpp>
#include <module-audio/Audio/Profiles/Profile.hpp>

#include <functional>

namespace style::window::volume
{
    constexpr inline auto title_height       = 33;
    constexpr inline auto base_title_key     = "app_popup_volume_text";
    constexpr inline auto bt_audio_title_key = "app_popup_bt_volume_text";
    constexpr inline auto music_title_key    = "app_popup_music_volume_text";
    constexpr inline auto call_title_key     = "app_popup_call_volume_text";
    constexpr inline auto mute_title_key     = "app_popup_muted_text";

    namespace bar
    {
        constexpr inline auto volume_levels = 10;
        constexpr inline auto top_offset    = 179;
    } // namespace bar

} // namespace style::window::volume

namespace app
{
    class ApplicationCommon;
} // namespace app

namespace gui
{
    class VolumeWindow : public WindowWithTimer
    {
      private:
        audio::Volume volume = 0;
        audio::Context audioContext;
        audio::VolumeChangeRequestSource source;

        void showProperText(const audio::Context &audioContext,
                            audio::Volume volume,
                            audio::VolumeChangeRequestSource source) noexcept;
        void showBaseTitle() noexcept;
        void showBTAudioDeviceVolume() noexcept;
        void showMultimediaPlayback() noexcept;
        void showCalling() noexcept;
        void showMuted() noexcept;

      protected:
        Label *volumeText    = nullptr;
        VBarGraph *volumeBar = nullptr;

      public:
        VolumeWindow(app::ApplicationCommon *app, const std::string &name);

        void addVolumeText();
        void addVolumeBar();
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;
    };
} // namespace gui
