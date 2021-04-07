// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Operation.hpp"

#include <Audio/AudioDevice.hpp>
#include <Audio/encoder/Encoder.hpp>
#include <Audio/AudioCommon.hpp>
#include <Audio/Profiles/Profile.hpp>
#include <Audio/Endpoint.hpp>

#include <mutex.hpp>

#include <memory>
#include <functional>
#include <string>
#include <vector>

#include <cstdint>

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
        static constexpr auto minimumBlockSize = 64U;
        static constexpr auto maximumBlockSize = 64U;
        static constexpr Endpoint::Capabilities routerCapabilities{.minBlockSize = minimumBlockSize,
                                                                   .maxBlockSize = maximumBlockSize};
        Mute mute = Mute::Disabled;

        void Mute();
        void Unmute();
        [[nodiscard]] auto IsMuted() const noexcept -> bool;

        std::unique_ptr<Stream> dataStreamOut;
        std::unique_ptr<Stream> dataStreamIn;
        std::unique_ptr<Encoder> enc;
        std::shared_ptr<AudioDevice> audioDeviceCellular;
        std::unique_ptr<StreamConnection> outputConnection;
        std::unique_ptr<StreamConnection> inputConnection;
    };

} // namespace audio
