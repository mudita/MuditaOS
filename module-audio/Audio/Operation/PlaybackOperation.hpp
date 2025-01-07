// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "Operation.hpp"
#include "Audio/Stream.hpp"
#include "Audio/Endpoint.hpp"
#include "Audio/decoder/DecoderWorker.hpp"
#include "Audio/decoder/Decoder.hpp"

#include <chrono>

using namespace std::chrono_literals;

namespace audio
{
    class PlaybackOperation : public Operation
    {
      public:
        PlaybackOperation(const std::string &filePath,
                          const PlaybackType &playbackType,
                          const PlaybackMode &playbackMode,
                          AudioServiceMessage::Callback callback = nullptr);

        ~PlaybackOperation() override;

        RetCode Start(Token token) final;
        RetCode Stop() final;
        RetCode Pause() final;
        RetCode Resume() final;
        RetCode SendEvent(std::shared_ptr<Event> evt) final;
        RetCode SwitchProfile(const Profile::Type type) final;
        RetCode SetOutputVolume(float vol) final;
        RetCode SetInputGain(float gain) final;

        Position GetPosition() final;
        RetCode SwitchToPriorityProfile(PlaybackType playbackType) final;

      private:
        static constexpr auto playbackTimeConstraint = 10ms;
        PlaybackMode playbackMode                    = PlaybackMode::Single;

        std::unique_ptr<Stream> dataStreamOut;
        std::unique_ptr<Decoder> dec;
        std::unique_ptr<StreamConnection> outputConnection;

        DecoderWorker::EndOfFileCallback endOfFileCallback;
        DecoderWorker::FileDeletedCallback fileDeletedCallback;
    };
} // namespace audio
