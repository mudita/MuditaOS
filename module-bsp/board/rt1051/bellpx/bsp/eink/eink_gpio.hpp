// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace bsp::eink
{
    /**
     * eink_gpio_configure
     *
     * This method initializes all GPIO pins needed for eInk handling.
     */

    void eink_gpio_configure();

    /**
     * eink_gpio_power_on
     *
     * This method physically turns on display power.
     */

    void eink_gpio_power_on();

    /**
     * eink_gpio_power_off
     *
     * This method physically turns off display power.
     */

    void eink_gpio_power_off();

} // namespace bsp::eink
