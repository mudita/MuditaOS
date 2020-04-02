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
    AudioMessage(MessageType messageType) : sys::DataMessage(messageType), type(messageType){};

    virtual ~AudioMessage(){};

    MessageType type;
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
};

class AudioResponseMessage : public sys::ResponseMessage
{
  public:
    AudioResponseMessage(audio::RetCode retCode  = audio::RetCode::Success,
                         const audio::Tags &tags = {},
                         const float val         = 0)
        : sys::ResponseMessage(), tags(tags), val(val)
    {}
    AudioResponseMessage(audio::RetCode retCode, const float val) : AudioResponseMessage(retCode, {}, val)
    {}
    virtual ~AudioResponseMessage()
    {}

    audio::RetCode retCode = audio::RetCode::Success;
    audio::Tags tags       = {};
    float val              = 0;
};

#endif // PUREPHONE_AUDIOMESSAGE_HPP
