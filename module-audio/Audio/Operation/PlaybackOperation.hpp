#pragma once

#include "Operation.hpp"

#include <bsp/audio/bsp_audio.hpp>

namespace audio
{
    class decoder;

    class PlaybackOperation : public Operation
    {
      public:
        PlaybackOperation(const char *file);

        audio::RetCode Start(std::function<int32_t(AudioEvents event)> callback) override final;

        int32_t Stop() override final;

        int32_t Pause() override final;

        int32_t Resume() override final;

        int32_t SendEvent(const Event evt, const EventData *data = nullptr) override final;

        int32_t SwitchProfile(const Profile::Type type) override final;

        int32_t SetOutputVolume(float vol) override final;

        int32_t SetInputGain(float gain) override final;

        Position GetPosition() override final;

      private:
        std::unique_ptr<decoder> dec                  = nullptr;
        std::unique_ptr<bsp::AudioDevice> audioDevice = nullptr;
    };

} // namespace audio
