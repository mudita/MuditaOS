// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
        static constexpr auto crossImg            = "cross";
        static constexpr auto addContactStr       = "app_call_contact";
        static const inline Icon::IconMap iconMap = {{AddContactIconState::ADD_CONTACT, {crossImg, addContactStr}}};

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
        static constexpr auto messageImg          = "mail";
        static constexpr auto sendSmstStr         = "app_call_message";
        static const inline Icon::IconMap iconMap = {{SendSmsIconState::SEND_SMS, {messageImg, sendSmstStr}}};

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
        static constexpr auto muteImg             = "microphone_on";
        static constexpr auto mutedImg            = "microphone_off";
        static constexpr auto muteStr             = "app_call_mute";
        static constexpr auto mutedStr            = "app_call_muted";
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
        static constexpr auto speakerImg   = "speaker_off";
        static constexpr auto speakerOnImg = "speaker_on";
        // static constexpr auto bluetoothImg = "app_call_bluetooth";
        static constexpr auto speakerStr   = "app_call_speaker";
        static constexpr auto speakerOnStr = "app_call_speaker_on";
        // static constexpr auto bluetoothStr = "app_call_bluetooth";
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
