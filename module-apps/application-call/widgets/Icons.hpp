#pragma once

#include "Icon.hpp"
#include "i18/i18.hpp"
#include "application-call/data/CallAppStyle.hpp"

namespace gui
{
    enum class AddContactIconState
    {
        ADD_CONTACT
    };
    class AddContactIcon : public Icon<AddContactIconState>
    {
      protected:
        static const inline std::string crossImg      = "cross";
        static const inline std::string addContactStr = "app_call_contact";
        static const inline Icon::IconMap iconMap     = {{AddContactIconState::ADD_CONTACT, {crossImg, addContactStr}}};

      public:
        AddContactIcon() = delete;
        AddContactIcon(Item *parent, const uint32_t &x, const uint32_t &y)
            : Icon(parent, x, y, callAppStyle::icon::x_margin, AddContactIconState::ADD_CONTACT, iconMap)
        {}
        ~AddContactIcon() override = default;
    };

    enum class SendSmsIconState
    {
        SEND_SMS
    };
    class SendSmsIcon : public Icon<SendSmsIconState>
    {
      protected:
        static const inline std::string messageImg  = "mail";
        static const inline std::string sendSmstStr = "app_call_message";
        static const inline Icon::IconMap iconMap   = {{SendSmsIconState::SEND_SMS, {messageImg, sendSmstStr}}};

      public:
        SendSmsIcon() = delete;
        SendSmsIcon(Item *parent, const uint32_t &x, const uint32_t &y)
            : Icon(parent, x, y, callAppStyle::icon::x_margin, SendSmsIconState::SEND_SMS, iconMap)
        {}
        ~SendSmsIcon() override = default;
    };

    enum class MicrophoneIconState
    {
        MUTE,
        MUTED
    };
    class MicrophoneIcon : public Icon<MicrophoneIconState>
    {
      protected:
        static const inline std::string muteImg   = "microphone_on";
        static const inline std::string mutedImg  = "microphone_off";
        static const inline std::string muteStr   = "app_call_mute";
        static const inline std::string mutedStr  = "app_call_muted";
        static const inline Icon::IconMap iconMap = {{MicrophoneIconState::MUTE, {muteImg, muteStr}},
                                                     {MicrophoneIconState::MUTED, {mutedImg, mutedStr}}};

      public:
        MicrophoneIcon() = delete;
        MicrophoneIcon(Item *parent, const uint32_t &x, const uint32_t &y)
            : Icon(parent, x, y, callAppStyle::icon::x_margin, MicrophoneIconState::MUTE, iconMap)
        {}
        ~MicrophoneIcon() override = default;
    };

    enum class SpeakerIconState
    {
        SPEAKER,
        SPEAKERON,
        // BLUETOOTH
    };

    class SpeakerIcon : public Icon<SpeakerIconState>
    {
      protected:
        static const inline std::string speakerImg   = "speaker_off";
        static const inline std::string speakerOnImg = "speaker_on";
        // static const inline std::string bluetoothImg = "app_call_bluetooth";
        static const inline std::string speakerStr   = "app_call_speaker";
        static const inline std::string speakerOnStr = "app_call_speaker_on";
        // static const inline std::string bluetoothStr = "app_call_bluetooth";
        static const inline Icon::IconMap iconMap = {
            {SpeakerIconState::SPEAKER, {speakerImg, speakerStr}},
            {SpeakerIconState::SPEAKERON, {speakerOnImg, speakerOnStr}},
            //{SpeakerIconState::BLUETOOTH, {bluetoothImg, bluetoothStr}}
        };

      public:
        SpeakerIcon() = delete;
        SpeakerIcon(Item *parent, const uint32_t &x, const uint32_t &y)
            : Icon(parent, x, y, callAppStyle::icon::x_margin, SpeakerIconState::SPEAKER, iconMap)
        {}
        ~SpeakerIcon() override = default;
    };
} // namespace gui
