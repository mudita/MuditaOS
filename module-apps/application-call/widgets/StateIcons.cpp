// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallAppStyle.hpp"
#include "StateIcons.hpp"

namespace gui
{
    namespace
    {
        constexpr auto crossImg      = "call_add";
        constexpr auto addContactStr = "app_call_contact";
        constexpr auto messageImg    = "call_message";
        constexpr auto sendSmstStr   = "app_call_message";
        constexpr auto muteImg       = "call_mute";
        constexpr auto mutedImg      = "call_mute_on";
        constexpr auto muteStr       = "app_call_mute";
        constexpr auto mutedStr      = "app_call_muted";
        constexpr auto speakerImg    = "call_speaker";
        constexpr auto speakerOnImg  = "call_speaker_on";
        constexpr auto speakerStr    = "app_call_speaker";
        constexpr auto speakerOnStr  = "app_call_speaker_on";

        const StateIcon<AddContactIconState>::IconMap contactIconMap = {
            {AddContactIconState::ADD_CONTACT, {crossImg, addContactStr}}};
        const StateIcon<SendSmsIconState>::IconMap smsIconMap = {
            {SendSmsIconState::SEND_SMS, {messageImg, sendSmstStr}}};
        const StateIcon<MicrophoneIconState>::IconMap microphoneIconMap = {
            {MicrophoneIconState::MUTE, {muteImg, muteStr}}, {MicrophoneIconState::MUTED, {mutedImg, mutedStr}}};
        const StateIcon<SpeakerIconState>::IconMap speakerIconMap = {
            {SpeakerIconState::SPEAKER, {speakerImg, speakerStr}},
            {SpeakerIconState::SPEAKERON, {speakerOnImg, speakerOnStr}}};
    } // namespace

    AddContactIcon::AddContactIcon(Item *parent) : StateIcon(parent, AddContactIconState::ADD_CONTACT, contactIconMap)
    {}

    SendSmsIcon::SendSmsIcon(Item *parent) : StateIcon(parent, SendSmsIconState::SEND_SMS, smsIconMap)
    {}

    MicrophoneIcon::MicrophoneIcon(Item *parent) : StateIcon(parent, MicrophoneIconState::MUTE, microphoneIconMap)
    {}

    SpeakerIcon::SpeakerIcon(Item *parent) : StateIcon(parent, SpeakerIconState::SPEAKER, speakerIconMap)
    {}

} // namespace gui
