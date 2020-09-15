/*
 *  @file AudioMessage.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 29.07.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#ifndef PUREPHONE_AUDIOMESSAGE_HPP
#define PUREPHONE_AUDIOMESSAGE_HPP

#include <memory>
#include <variant>
#include "Service/Message.hpp"
#include "MessageType.hpp"
#include "Audio/AudioCommon.hpp"
#include "Audio/decoder/decoder.hpp"

class AudioMessage : public sys::DataMessage
{
  public:
    AudioMessage(MessageType messageType) : sys::DataMessage(messageType), type(messageType)
    {}

    AudioMessage() : sys::DataMessage(MessageType::MessageTypeUninitialized)
    {}

    virtual ~AudioMessage() = default;

    MessageType type = MessageType::MessageTypeUninitialized;
};

class AudioNotificationMessage : public AudioMessage
{
  public:
    // Audio service will send async notifications specified below
    enum class Type
    {
        EndOfFile,
        Stop
    };

    AudioNotificationMessage(Type type) : AudioMessage(MessageType::AudioNotification), type(type)
    {}

    ~AudioNotificationMessage()
    {}

    Type type;
};

class AudioSettingsMessage : public AudioMessage
{
  public:
    AudioSettingsMessage(const audio::Profile::Type &profileType,
                         const audio::PlaybackType &playbackType,
                         const uint32_t &val = 0)
        : AudioMessage{}, profileType{profileType}, playbackType{playbackType}, val{val}
    {}

    ~AudioSettingsMessage() override = default;

    audio::Profile::Type profileType = audio::Profile::Type::Idle;
    audio::PlaybackType playbackType = audio::PlaybackType::None;
    uint32_t val{};
};

class AudioGetSetting : public AudioSettingsMessage
{
  public:
    AudioGetSetting(const audio::Profile::Type &profileType, const audio::PlaybackType &playbackType)
        : AudioSettingsMessage{profileType, playbackType}
    {}

    ~AudioGetSetting() override = default;
};

class AudioSetSetting : public AudioSettingsMessage
{
  public:
    AudioSetSetting(const audio::Profile::Type &profileType,
                    const audio::PlaybackType &playbackType,
                    const uint32_t &val)
        : AudioSettingsMessage{profileType, playbackType, val}
    {}

    ~AudioSetSetting() override = default;
};

class AudioRequestMessage : public AudioMessage
{
  public:
    AudioRequestMessage(MessageType messageType) : AudioMessage(messageType)
    {}
    ~AudioRequestMessage()
    {}

    std::string fileName;
    float val;
    bool enable;
    audio::PlaybackType playbackType = audio::PlaybackType::None;
};

class AudioResponseMessage : public sys::ResponseMessage
{
  public:
    AudioResponseMessage(audio::RetCode retCode  = audio::RetCode::Success,
                         const audio::Tags &tags = {},
                         const float val         = 0.0)
        : sys::ResponseMessage(), tags(tags), val(val)
    {}
    AudioResponseMessage(audio::RetCode retCode, const float val) : AudioResponseMessage(retCode, {}, val)
    {}
    virtual ~AudioResponseMessage()
    {}

    audio::RetCode retCode = audio::RetCode::Success;
    audio::Tags tags       = {};
    float val              = 0.0;
};

#endif // PUREPHONE_AUDIOMESSAGE_HPP
