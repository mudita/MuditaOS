#pragma once

#include "Operation.hpp"

#include <bsp/audio/bsp_audio.hpp>

namespace audio
{
    class decoder;

    class PlaybackOperation : public Operation
    {
      public:
        PlaybackOperation(
            const char *file,
            std::function<uint32_t(const std::string &path, const uint32_t &defaultValue)> dbCallback = nullptr);

        audio::RetCode Start(std::function<int32_t(AudioEvents event)> callback) override final;

        audio::RetCode Stop() override final;

        audio::RetCode Pause() override final;

        audio::RetCode Resume() override final;

        audio::RetCode SendEvent(const Event evt, const EventData *data = nullptr) override final;

        audio::RetCode SwitchProfile(const Profile::Type type) override final;

        audio::RetCode SetOutputVolume(float vol) override final;

        audio::RetCode SetInputGain(float gain) override final;

        Position GetPosition() override final;

      private:
        std::unique_ptr<decoder> dec                  = nullptr;
        std::unique_ptr<bsp::AudioDevice> audioDevice = nullptr;
    };

} // namespace audio
