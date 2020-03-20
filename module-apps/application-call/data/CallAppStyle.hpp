#pragma once

#include <Style.hpp>

namespace callAppStyle
{
    namespace strings
    {
        const inline std::string call      = "app_call_call";
        const inline std::string clear     = "app_call_clear";
        const inline std::string reject    = "app_call_reject";
        const inline std::string answer    = "app_call_answer";
        const inline std::string message   = "app_call_message";
        const inline std::string endcall   = "app_call_end_call";
        const inline std::string emergency = "app_call_emergency";
        const inline std::string iscalling = "app_call_is_calling";
        const inline std::string calling   = "app_call_calling";
        const inline std::string callended = "app_call_call_ended";
        const inline std::string add       = "app_call_add";
        const inline std::string contact   = "app_call_contact";
        const inline std::string mute      = "app_call_mute";
        const inline std::string MUTED     = "app_call_muted";
        const inline std::string speaker   = "app_call_speaker";
        const inline std::string speakeron = "app_call_speaker_on";
        const inline std::string bluetooth = "app_call_bluetooth";
    } // namespace strings

    namespace numberLabel
    {
        constexpr uint32_t x       = 60;
        constexpr uint32_t y       = 157;
        constexpr uint32_t w       = style::window_width - 2 * x;
        constexpr uint32_t h       = 51 + 16;
        constexpr uint32_t borderW = 1;
    } // namespace numberLabel

    namespace icon
    {
        constexpr uint32_t x_margin = 20;
    }

    // ENTER NUMBER WINDOW
    namespace enterNumberWindow
    {
        namespace newContactIcon
        {
            constexpr uint32_t x = 190 - icon::x_margin;
            constexpr uint32_t y = 411;
        } // namespace newContactIcon
    }     // namespace enterNumberWindow

    // CALL WINDOW
    namespace callWindow
    {
        namespace imageCircleTop
        {
            constexpr uint32_t x = 116; // TODO: should be 104 with final image
            constexpr uint32_t y = 59;
            constexpr auto name  = "circle_top";
        } // namespace imageCircleTop
        namespace imageCircleBottom
        {
            constexpr uint32_t x = 106; // TODO: should be 104 with final image
            constexpr uint32_t y = 240;
            constexpr auto name  = "circle_bottom";
        } // namespace imageCircleBottom
        namespace durationLabel
        {
            constexpr uint32_t x = 120;
            constexpr uint32_t y = 223;
            constexpr uint32_t w = 240;
            constexpr uint32_t h = 20;
        } // namespace durationLabel
        namespace speakerIcon
        {
            constexpr uint32_t x = 260 - icon::x_margin;
            constexpr uint32_t y = 411;
        } // namespace speakerIcon
        namespace microphoneIcon
        {
            constexpr uint32_t x = 120 - icon::x_margin;
            constexpr uint32_t y = 411;
        } // namespace microphoneIcon
        namespace sendMessageIcon
        {
            constexpr uint32_t x = 190 - icon::x_margin;
            constexpr uint32_t y = 411;
        } // namespace sendMessageIcon
    }     // namespace callWindow
} // namespace callAppStyle