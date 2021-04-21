// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Audio/AudioCommon.hpp>
#include <Audio/decoder/Decoder.hpp>
#include <MessageType.hpp>
#include <service-appmgr/service-appmgr/messages/ActionRequest.hpp>
#include <module-services/service-appmgr/service-appmgr/Actions.hpp>
#include <module-apps/popups/data/PopupRequestParams.hpp>

#include <Service/Message.hpp>

#include <memory>
#include <variant>

class AudioMessage : public sys::DataMessage
{
  public:
    AudioMessage() : sys::DataMessage(MessageType::AudioMessage)
    {}

    virtual ~AudioMessage() = default;
};

class AudioResponseMessage : public sys::ResponseMessage
{
  public:
    explicit AudioResponseMessage(audio::RetCode retCode  = audio::RetCode::Success,
                                  const audio::Tags &tags = {},
                                  const std::string &val  = {})
        : sys::ResponseMessage(), retCode(retCode), tags(tags), val(val)
    {}

    AudioResponseMessage(audio::RetCode retCode, const std::string &val) : AudioResponseMessage(retCode, {}, val)
    {}

    virtual ~AudioResponseMessage() = default;

    const audio::RetCode retCode = audio::RetCode::Success;
    audio::Tags tags             = {};
    std::string val;
};

class AudioNotificationMessage : public AudioMessage
{
  public:
    // Audio service will send async notifications specified below
    enum class Type
    {
        EndOfFile,
        Stop,
        ServiceWakeUp,
        ServiceSleep,
    };

    explicit AudioNotificationMessage(Type type, const audio::Token token = audio::Token()) : type(type), token(token)
    {}

    const Type type;
    const audio::Token token;
};

class AudioSettingsMessage : public AudioMessage
{
  public:
    AudioSettingsMessage(const audio::PlaybackType &playbackType,
                         const audio::Setting &setting,
                         const std::string &val = {})
        : AudioMessage{}, playbackType{playbackType}, setting{setting}, val{val}
    {}

    ~AudioSettingsMessage() override = default;

    audio::PlaybackType playbackType = audio::PlaybackType::None;
    const audio::Setting setting;
    std::string val{};
};

class AudioGetSetting : public AudioSettingsMessage
{
  public:
    AudioGetSetting(const audio::PlaybackType &playbackType, const audio::Setting &setting)
        : AudioSettingsMessage{playbackType, setting}
    {}

    ~AudioGetSetting() override = default;
};

class AudioSetSetting : public AudioSettingsMessage
{
  public:
    AudioSetSetting(const audio::PlaybackType &playbackType, const audio::Setting &setting, const std::string &val)
        : AudioSettingsMessage{playbackType, setting, val}
    {}

    ~AudioSetSetting() override = default;
};

class AudioStopRequest : public AudioMessage
{
  public:
    AudioStopRequest(const std::vector<audio::PlaybackType> &stopVec = {}) : stopVec(stopVec)
    {}

    AudioStopRequest(const audio::Token &token) : token(token)
    {}

    const std::vector<audio::PlaybackType> stopVec;
    const audio::Token token;
};

class AudioStopResponse : public AudioResponseMessage
{
  public:
    AudioStopResponse(audio::RetCode retCode, const audio::Token &token) : AudioResponseMessage(retCode), token(token)
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

class AudioStartRoutingRequest : public AudioMessage
{
  public:
    AudioStartRoutingRequest()
    {}
};

class AudioStartRoutingResponse : public AudioResponseMessage
{
  public:
    AudioStartRoutingResponse(audio::RetCode retCode, const audio::Token &token)
        : AudioResponseMessage(retCode), token(token)
    {}

    const audio::Token token;
};

class AudioStartRecorderRequest : public AudioMessage
{
  public:
    AudioStartRecorderRequest(const std::string &fileName) : fileName(fileName)
    {}

    const std::string fileName;
};

class AudioStartRecorderResponse : public AudioResponseMessage
{
  public:
    AudioStartRecorderResponse(audio::RetCode retCode, const audio::Token &token)
        : AudioResponseMessage(retCode), token(token)
    {}

    const audio::Token token;
};

class AudioPauseRequest : public AudioMessage
{
  public:
    AudioPauseRequest(const audio::Token &token) : token(token)
    {}

    const audio::Token token;
};

class AudioPauseResponse : public AudioResponseMessage
{
  public:
    AudioPauseResponse(audio::RetCode retCode, const audio::Token &token) : AudioResponseMessage(retCode), token(token)
    {}

    const audio::Token token;
};

class AudioResumeRequest : public AudioMessage
{
  public:
    AudioResumeRequest(const audio::Token &token) : token(token)
    {}

    const audio::Token token;
};

class AudioResumeResponse : public AudioResponseMessage
{
  public:
    AudioResumeResponse(audio::RetCode retCode, const audio::Token &token) : AudioResponseMessage(retCode), token(token)
    {}

    const audio::Token token;
};

class AudioGetFileTagsRequest : public AudioMessage
{
  public:
    AudioGetFileTagsRequest(const std::string &fileName) : fileName(fileName)
    {}

    const std::string fileName;
};

class AudioEventRequest : public AudioMessage
{
  public:
    explicit AudioEventRequest(std::shared_ptr<audio::Event> evt) : evt(std::move(evt))
    {}

    explicit AudioEventRequest(audio::EventType eType, audio::Event::DeviceState state)
        : evt(std::make_shared<audio::Event>(eType, state))
    {}

    std::shared_ptr<audio::Event> getEvent()
    {
        return evt;
    }

  private:
    std::shared_ptr<audio::Event> evt;
};

class AudioEventResponse : public AudioResponseMessage
{
  public:
    using AudioResponseMessage::AudioResponseMessage;
};

class AudioKeyPressedRequest : public AudioMessage
{
  public:
    AudioKeyPressedRequest(const int step) : AudioMessage{}, step{step}
    {}
    const int step{};
};

class VolumeChanged : public sys::DataMessage, public app::manager::actions::ConvertibleToAction
{
  public:
    VolumeChanged(audio::Volume volume, audio::Context context)
        : sys::DataMessage{MessageType::MessageTypeUninitialized}, volume{volume}, context{context}
    {}

    [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest> override
    {
        return std::make_unique<app::manager::ActionRequest>(
            sender, app::manager::actions::ShowPopup, std::make_unique<gui::VolumePopupRequestParams>(volume, context));
    }

  private:
    const audio::Volume volume;
    audio::Context context;
};

class BluetoothDeviceVolumeChanged : public AudioMessage
{
  public:
    explicit BluetoothDeviceVolumeChanged(std::uint8_t volume) : volume{volume}
    {}

    [[nodiscard]] auto getVolume() const noexcept -> std::uint8_t
    {
        return volume;
    }

  private:
    const std::uint8_t volume;
};
