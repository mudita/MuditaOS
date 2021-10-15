// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <chrono>

#include <MessageType.hpp>
#include <Service/Message.hpp>
#include <bsp/common.hpp>

namespace sys
{
    class Service;
} // namespace sys

namespace EventManagerServiceAPI
{
    /// Gets board version
    bsp::Board GetBoard(sys::Service *serv);

    /// Call single vibrator pulse
    void vibrationPulseOnce(sys::Service *serv);

    /// Call vibrator to stop
    void vibrationStop(sys::Service *serv);

    /// Call repetitive vibrator pulses for given time [ms]
    void vibrationPulseRepeat(sys::Service *serv, std::chrono::milliseconds time);

    /// Call repetitive vibrator pulses until stop message is sent
    void vibrationPulseRepeatUntilStop(sys::Service *serv);

    /// Set vibrator level
    void setVibrationLevel(sys::Service *serv, unsigned int vibrationLevel);

} // namespace EventManagerServiceAPI
