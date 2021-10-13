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
    /*
     * @brief Gets board version.
     * @return board type
     */
    bsp::Board GetBoard(sys::Service *serv);

    /*
     * @brief Call single vibra pulse
     */
    void vibrationPulseOnce(sys::Service *serv);

    /*
     * @brief Call vibra to stop
     */
    void vibrationStop(sys::Service *serv);

    /*
     * @brief Call repetitive vibra pulses for given time [ms]
     */
    void vibrationPulseRepeat(sys::Service *serv, std::chrono::milliseconds time);

    /*
     * @brief Call repetitive vibra pulses until stop message is sent
     */
    void vibrationPulseRepeatUntilStop(sys::Service *serv);

    /*
     * @brief Set vibration level
     */
    void setVibrationLevel(sys::Service *serv, unsigned int vibrationLevel);

} // namespace EventManagerServiceAPI
