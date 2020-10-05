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

#include "Service/Bus.hpp"

class AudioMessage : public std::enable_shared_from_this<AudioMessage>, public sys::DataMessage
{
  public:
    AudioMessage() : sys::DataMessage(MessageType::MessageTypeUninitialized)
    {}

    AudioMessage(MessageType messageType,
                 audio::PlaybackType playbackType = audio::PlaybackType::None,
                 audio::Token token               = audio::Token())
        : sys::DataMessage(messageType), type(messageType), playbackType(playbackType), token(token)
    {}

    virtual ~AudioMessage() = default;
    MessageType type = MessageType::MessageTypeUninitialized;
    audio::PlaybackType playbackType = audio::PlaybackType::None;
    audio::Token token;
};

class AudioResponseMessage : public sys::ResponseMessage
{
  public:
    AudioResponseMessage(audio::RetCode retCode  = audio::RetCode::Success,
                         const audio::Tags &tags = {},
                         audio::Token token      = audio::Token(),
                         const float val         = 0.0)
        : sys::ResponseMessage(), tags(tags), val(val), token(token)
    {}

    AudioResponseMessage(audio::RetCode retCode, const float val)
        : AudioResponseMessage(retCode, {}, audio::Token(), val)
    {}

    virtual ~AudioResponseMessage()
    {}

    audio::RetCode retCode = audio::RetCode::Success;
    audio::Tags tags       = {};
    float val              = 0.0;
    audio::Token token;
};

class AudioRequestSync : virtual public AudioMessage
{
  public:
    AudioRequestSync()
    {}
    virtual ~AudioRequestSync()
    {}
    using enable_shared_from_this<AudioMessage>::shared_from_this;
    audio::Handle SendSync(sys::Service *serv);
};

class AudioRequestAsync : virtual public AudioMessage
{
  public:
    AudioRequestAsync()
    {}
    virtual ~AudioRequestAsync()
    {}
    using enable_shared_from_this<AudioMessage>::shared_from_this;
    bool SendAsync(sys::Service *serv);
};

class AudioRequest : public AudioRequestAsync, public AudioRequestSync
{
  public:
    AudioRequest()
    {}
    virtual ~AudioRequest()
    {}
};

class PlaybackStartReq : public AudioRequest
{
  public:
    PlaybackStartReq(const std::string fileName, const audio::PlaybackType playbackType)
        : fileName(fileName), playbackType(playbackType)
    {}

    ~PlaybackStartReq()
    {}

    const std::string fileName;
    const audio::PlaybackType playbackType;
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

    AudioNotificationMessage(Type type, audio::Token token)
        : AudioMessage(MessageType::AudioNotification, audio::PlaybackType::None, token), type(type)
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
                         const audio::Setting &setting,
                         const uint32_t &val = 0)
        : AudioMessage{}, profileType{profileType}, playbackType{playbackType}, setting{setting}, val{val}
    {}

    ~AudioSettingsMessage() override = default;

    audio::Profile::Type profileType = audio::Profile::Type::Idle;
    audio::PlaybackType playbackType = audio::PlaybackType::None;
    const audio::Setting setting;
    uint32_t val{};
};

class AudioGetSetting : public AudioSettingsMessage
{
  public:
    AudioGetSetting(const audio::Profile::Type &profileType,
                    const audio::PlaybackType &playbackType,
                    const audio::Setting &setting)
        : AudioSettingsMessage{profileType, playbackType, setting}
    {}

    ~AudioGetSetting() override = default;
};

class AudioSetSetting : public AudioSettingsMessage
{
  public:
    AudioSetSetting(const audio::Profile::Type &profileType,
                    const audio::PlaybackType &playbackType,
                    const audio::Setting &setting,
                    const uint32_t &val)
        : AudioSettingsMessage{profileType, playbackType, setting, val}
    {}

    ~AudioSetSetting() override = default;
};

class AudioRequestMessage : public AudioMessage
{
  public:
    AudioRequestMessage(MessageType messageType, audio::Token token = audio::Token())
        : AudioMessage(messageType, audio::PlaybackType::None, token)
    {}

    ~AudioRequestMessage()
    {}

    std::string fileName;
    float val;
    bool enable;
};

class AudioStopMessage : public AudioMessage
{
  public:
    AudioStopMessage(const std::vector<audio::PlaybackType> &stopVec = {}) : AudioMessage(), stopVec(stopVec)
    {}

    AudioStopMessage(const audio::Token &token) : AudioMessage(), token(token)
    {}

    const std::vector<audio::PlaybackType> stopVec;
    const audio::Token token;
};

#endif // PUREPHONE_AUDIOMESSAGE_HPP
