// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Operation.hpp"

#include <bsp/audio/bsp_audio.hpp>

namespace audio::playbackDefaults
{
    constexpr audio::Volume defaultLoudspeakerVolume = 10;
    constexpr audio::Volume defaultHeadphonesVolume  = 2;
} // namespace audio::playbackDefaults

namespace audio
{
    class decoder;
    class Tags;

    class PlaybackOperation : public Operation
    {
      public:
        PlaybackOperation(
            const char *file,
            const audio::PlaybackType &playbackType,
            std::function<uint32_t(const std::string &path, const uint32_t &defaultValue)> dbCallback = nullptr);

        virtual ~PlaybackOperation();

        audio::RetCode Start(audio::AsyncCallback callback, audio::Token token) final;
        audio::RetCode Stop() final;
        audio::RetCode Pause() final;
        audio::RetCode Resume() final;
        audio::RetCode SendEvent(std::shared_ptr<Event> evt) final;
        audio::RetCode SwitchProfile(const Profile::Type type) final;
        audio::RetCode SetOutputVolume(float vol) final;
        audio::RetCode SetInputGain(float gain) final;

        Position GetPosition() final;
        void SetBluetoothStreamData(BluetoothStreamData data) final;

      private:
        std::unique_ptr<decoder> dec;
        std::unique_ptr<bsp::AudioDevice> audioDevice;
        std::unique_ptr<Tags> tags;
    };

} // namespace audio
