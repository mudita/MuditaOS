// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Icon.hpp"

#include <cstdint>

namespace gui
{
    enum class AddContactIconState
    {
        ADD_CONTACT
    };

    class AddContactIcon : public Icon<AddContactIconState>
    {
      public:
        AddContactIcon() = delete;
        AddContactIcon(Item *parent, std::uint32_t x, std::uint32_t y);
    };

    enum class SendSmsIconState
    {
        SEND_SMS
    };
    class SendSmsIcon : public Icon<SendSmsIconState>
    {
      public:
        SendSmsIcon() = delete;
        SendSmsIcon(Item *parent, std::uint32_t x, std::uint32_t y);
    };

    enum class MicrophoneIconState
    {
        MUTE,
        MUTED
    };

    class MicrophoneIcon : public Icon<MicrophoneIconState>
    {
      public:
        MicrophoneIcon() = delete;
        MicrophoneIcon(Item *parent, std::uint32_t x, std::uint32_t y);
    };

    enum class SpeakerIconState
    {
        SPEAKER,
        SPEAKERON
    };

    class SpeakerIcon : public Icon<SpeakerIconState>
    {
      public:
        SpeakerIcon() = delete;
        SpeakerIcon(Item *parent, std::uint32_t x, std::uint32_t y);
    };
} // namespace gui
