// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "bsp/vibrator/vibrator.hpp" // for vibrator State
#include "bsp/common.hpp"            // for Board

#include <MessageType.hpp>
#include <Service/Message.hpp>

#include <chrono>

namespace sys
{
    class Service;
} // namespace sys

namespace EventManagerServiceAPI
{
    /*
     * @brief Gets board version.
     * @return board type
     */
    bsp::Board GetBoard(sys::Service *serv);

    /// @brief Set a steady vibration or a lack thereof
    void TurnVibration(sys::Service *serv, bsp::vibrator::State state);

    /// @brief Vibrate a pulse of vibration
    /// @param pulseDuration duration of the vibration pulse
    /// @param repeatEvery optional periodic repetition od the pulse
    void PulseVibration(sys::Service *serv,
                        std::chrono::milliseconds pulseDuration,
                        std::chrono::milliseconds repeatEvery = std::chrono::milliseconds::zero());
} // namespace EventManagerServiceAPI
