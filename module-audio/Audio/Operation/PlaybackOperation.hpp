/*
 *  @file PlaybackOperation.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 23.07.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#ifndef PUREPHONE_PLAYBACKOPERATION_HPP
#define PUREPHONE_PLAYBACKOPERATION_HPP

#include <memory>
#include <optional>
#include <functional>
#include "bsp/audio/bsp_audio.hpp"

#include "Operation.hpp"

namespace audio
{

    class decoder;

    class PlaybackOperation : public Operation
    {
      public:
        PlaybackOperation(const char *file);

        int32_t Start(std::function<int32_t(AudioEvents event)> callback) override final;

        int32_t Stop() override final;

        int32_t Pause() override final;

        int32_t Resume() override final;

        int32_t SendEvent(const Event evt, const EventData *data = nullptr) override final;

        int32_t SwitchProfile(const Profile::Type type) override final;

        int32_t SetOutputVolume(float vol) override final;

        int32_t SetInputGain(float gain) override final;

        Position GetPosition() override final;

      private:
        std::unique_ptr<decoder> dec;
        std::unique_ptr<bsp::AudioDevice> audioDevice;
    };

} // namespace audio

#endif // PUREPHONE_PLAYBACKOPERATION_HPP
