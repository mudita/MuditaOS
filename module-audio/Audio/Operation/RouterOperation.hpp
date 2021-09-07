// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Operation.hpp"

#include <Audio/AudioDevice.hpp>
#include <Audio/AudioCommon.hpp>
#include <Audio/Profiles/Profile.hpp>
#include <Audio/Endpoint.hpp>
#include <Audio/Stream.hpp>

#include <mutex.hpp>

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <cstdint>

using namespace std::chrono_literals;

namespace audio
{
    class RouterOperation : public Operation
    {
        using AudioCallback =
            std::function<std::int32_t(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer)>;
        static const std::size_t INPUT_BUFFER_START_SIZE = 1024;
        enum class Mute : bool
        {
            Enabled,
            Disabled
        };

        enum class JackState : bool
        {
            Plugged,
            Unplugged
        };

      public:
        RouterOperation(const char *file, AudioServiceMessage::Callback callback);
        ~RouterOperation();

        audio::RetCode Start(audio::Token token) final;
        audio::RetCode Stop() final;
        audio::RetCode Pause() final;
        audio::RetCode Resume() final;
        audio::RetCode SendEvent(std::shared_ptr<Event> evt) final;
        audio::RetCode SwitchProfile(const Profile::Type type) final;
        audio::RetCode SetOutputVolume(float vol) final;
        audio::RetCode SetInputGain(float gain) final;

        Position GetPosition() final;

      private:
        static constexpr auto callTimeConstraint = 2ms;

        Mute mute           = Mute::Disabled;
        JackState jackState = JackState::Unplugged;

        void Mute();
        void Unmute();
        [[nodiscard]] auto IsMuted() const noexcept -> bool;

        std::unique_ptr<AbstractStream> dataStreamOut;
        std::unique_ptr<AbstractStream> dataStreamIn;
        std::shared_ptr<AudioDevice> audioDeviceCellular;
        std::unique_ptr<StreamConnection> voiceOutputConnection;
        std::unique_ptr<StreamConnection> voiceInputConnection;
    };

} // namespace audio
