﻿// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
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
                          const audio::PlaybackType &playbackType,
                          const audio::PlaybackMode &playbackMode,
                          AudioServiceMessage::Callback callback = nullptr);

        ~PlaybackOperation() override;

        audio::RetCode Start(audio::Token token) final;
        audio::RetCode Stop() final;
        audio::RetCode Pause() final;
        audio::RetCode Resume() final;
        audio::RetCode SendEvent(std::shared_ptr<Event> evt) final;
        audio::RetCode SwitchProfile(const Profile::Type type) final;
        audio::RetCode SetOutputVolume(float vol) final;
        audio::RetCode SetInputGain(float gain) final;

        Position GetPosition() final;
        audio::RetCode SwitchToPriorityProfile(audio::PlaybackType playbackType) final;

      private:
        static constexpr auto playbackTimeConstraint = 10ms;
        static constexpr auto playbackStartPosition  = 0U;
        audio::PlaybackMode playbackMode             = audio::PlaybackMode::Single;

        std::unique_ptr<Stream> dataStreamOut;
        std::unique_ptr<Decoder> dec;
        std::unique_ptr<StreamConnection> outputConnection;

        DecoderWorker::EndOfFileCallback endOfFileCallback;
        DecoderWorker::FileDeletedCallback fileDeletedCallback;
    };
} // namespace audio
