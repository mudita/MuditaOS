// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>

namespace callAppStyle
{
    namespace strings
    {
        inline constexpr auto call          = "app_call_call";
        inline constexpr auto clear         = "app_call_clear";
        inline constexpr auto reject        = "app_call_reject";
        inline constexpr auto answer        = "app_call_answer";
        inline constexpr auto message       = "app_call_message";
        inline constexpr auto endcall       = "app_call_end_call";
        inline constexpr auto emergency     = "app_call_emergency";
        inline constexpr auto iscalling     = "app_call_is_calling";
        inline constexpr auto calling       = "app_call_calling";
        inline constexpr auto callended     = "app_call_call_ended";
        inline constexpr auto callrejected  = "app_call_call_rejected";
        inline constexpr auto contact       = "app_call_contact";
        inline constexpr auto mute          = "app_call_mute";
        inline constexpr auto MUTED         = "app_call_muted";
        inline constexpr auto speaker       = "app_call_speaker";
        inline constexpr auto speakeron     = "app_call_speaker_on";
        inline constexpr auto bluetooth     = "app_call_bluetooth";
        inline constexpr auto privateNumber = "app_call_private_number";
    } // namespace strings

    namespace numberLabel
    {
        inline constexpr auto x       = 60U;
        inline constexpr auto y       = 157U;
        inline constexpr auto w       = style::window_width - 2 * x;
        inline constexpr auto h       = 51U + 16U;
        inline constexpr auto borderW = 1U;
    } // namespace numberLabel

    namespace numberDescriptionLabel
    {
        inline constexpr auto x = 60U;
        inline constexpr auto y = 254U;
        inline constexpr auto w = style::window_width - 2 * x;
        inline constexpr auto h = 20U;
    } // namespace numberDescriptionLabel

    namespace icon
    {
        inline constexpr auto x_margin = 20U;
    }

    // ENTER NUMBER WINDOW
    namespace enterNumberWindow
    {
        namespace newContactIcon
        {
            inline constexpr auto x = 190U - icon::x_margin;
            inline constexpr auto y = 411U;
        } // namespace newContactIcon
    }     // namespace enterNumberWindow

    // CALL WINDOW
    namespace callWindow
    {
        namespace imageCircleTop
        {
            inline constexpr auto x    = 111U;
            inline constexpr auto y    = 59U;
            inline constexpr auto name = "circle_top";
        } // namespace imageCircleTop
        namespace imageCircleBottom
        {
            inline constexpr auto x    = 104U;
            inline constexpr auto y    = 240U;
            inline constexpr auto name = "circle_bottom";
        } // namespace imageCircleBottom
        namespace durationLabel
        {
            inline constexpr auto x = 120U;
            inline constexpr auto y = 223U;
            inline constexpr auto w = 240U;
            inline constexpr auto h = 20U;
        } // namespace durationLabel
        namespace speakerIcon
        {
            inline constexpr auto x = 260U - icon::x_margin;
            inline constexpr auto y = 411U;
        } // namespace speakerIcon
        namespace microphoneIcon
        {
            inline constexpr auto x = 120U - icon::x_margin;
            inline constexpr auto y = 411U;
        } // namespace microphoneIcon
        namespace sendMessageIcon
        {
            inline constexpr auto x = 190U - icon::x_margin;
            inline constexpr auto y = 411U;
        } // namespace sendMessageIcon
    }     // namespace callWindow
} // namespace callAppStyle
