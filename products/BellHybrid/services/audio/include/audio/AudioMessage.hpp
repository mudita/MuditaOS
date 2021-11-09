// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Audio/AudioCommon.hpp>
#include <MessageType.hpp>
#include <Service/Message.hpp>

#include <memory>
#include <variant>

namespace service
{
    inline constexpr auto audioServiceName = "ServiceAudio";

    class AudioMessage : public sys::DataMessage
    {
      public:
        AudioMessage() : sys::DataMessage(MessageType::AudioMessage)
        {}
    };

    class AudioResponseMessage : public sys::ResponseMessage
    {
      public:
        explicit AudioResponseMessage(audio::RetCode retCode = audio::RetCode::Success, const std::string &val = {})
            : sys::ResponseMessage(), retCode(retCode), val(val)
        {}

        const audio::RetCode retCode = audio::RetCode::Success;
        std::string val;
    };

    class AudioNotificationMessage : public AudioMessage
    {
      public:
        explicit AudioNotificationMessage(audio::Token token) : token{token}
        {}

        const audio::Token token;
    };

    class AudioStopNotification : public AudioNotificationMessage
    {
      public:
        explicit AudioStopNotification(audio::Token token) : AudioNotificationMessage{token}
        {}
    };

    class AudioEOFNotification : public AudioNotificationMessage
    {
      public:
        explicit AudioEOFNotification(audio::Token token) : AudioNotificationMessage{token}
        {}
    };

    class AudioSettingsMessage : public AudioMessage
    {
      public:
        AudioSettingsMessage(const audio::PlaybackType &playbackType, const std::string &val = {})
            : AudioMessage{}, playbackType{playbackType}, val{val}
        {}

        audio::PlaybackType playbackType = audio::PlaybackType::None;
        std::string val{};
    };

    class AudioGetVolume : public AudioSettingsMessage
    {
      public:
        AudioGetVolume(const audio::PlaybackType &playbackType) : AudioSettingsMessage{playbackType}
        {}
    };

    class AudioSetVolume : public AudioSettingsMessage
    {
      public:
        AudioSetVolume(const audio::PlaybackType &playbackType, const std::string &val)
            : AudioSettingsMessage{playbackType, val}
        {}
    };

    class AudioStopRequest : public AudioMessage
    {
      public:
        explicit AudioStopRequest(const std::vector<audio::PlaybackType> &stopVec = {}) : stopVec(stopVec)
        {}

        explicit AudioStopRequest(const audio::Token &token) : token(token)
        {}

        const std::vector<audio::PlaybackType> stopVec;
        const audio::Token token;
    };

    class AudioStopResponse : public AudioResponseMessage
    {
      public:
        AudioStopResponse(audio::RetCode retCode, const audio::Token &token)
            : AudioResponseMessage(retCode), token(token)
        {}

        const audio::Token token;
    };

    class AudioStartPlaybackRequest : public AudioMessage
    {
      public:
        AudioStartPlaybackRequest(const std::string &fileName, const audio::PlaybackType &playbackType)
            : AudioMessage(), fileName(fileName), playbackType(playbackType)
        {}

        const std::string fileName;
        const audio::PlaybackType playbackType;
    };

    class AudioStartPlaybackResponse : public AudioResponseMessage
    {
      public:
        AudioStartPlaybackResponse(audio::RetCode retCode, const audio::Token &token)
            : AudioResponseMessage(retCode), token(token)
        {}

        const audio::Token token;
    };

    class AudioPauseRequest : public AudioMessage
    {
      public:
        AudioPauseRequest() : AudioMessage()
        {}
    };

    class AudioResumeRequest : public AudioMessage
    {
      public:
        AudioResumeRequest() : AudioMessage()
        {}
    };
} // namespace service
