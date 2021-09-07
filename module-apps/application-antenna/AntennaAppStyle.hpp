// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * AntennaAppStyle.hpp
 *
 *  Created on: 3 kwi 2020
 *      Author: kuba
 */

#ifndef MODULE_APPS_APPLICATION_ANTENNA_ANTENNAAPPSTYLE_HPP_
#define MODULE_APPS_APPLICATION_ANTENNA_ANTENNAAPPSTYLE_HPP_

#include <module-gui/gui/widgets/Style.hpp>

namespace antenna
{

    namespace main_window
    {
        constexpr uint32_t commonXPos    = 40;
        constexpr uint32_t commonYOffset = 38;

        constexpr uint32_t titleW = 400;
        constexpr uint32_t titleH = 33;

        constexpr uint32_t buttonH      = 33;
        constexpr uint32_t buttonSmallW = 130;
        constexpr uint32_t buttonBigW   = 400;
        constexpr uint32_t buttonMidW   = 195;

        constexpr uint32_t buttonPosXLeft   = 35;
        constexpr uint32_t buttonPosXCenter = 175;
        constexpr uint32_t buttonPostXRight = 315;

        constexpr uint32_t buttonPosXColumnLeft  = 40;
        constexpr uint32_t buttonPosXColumnRight = 240;

        constexpr uint32_t buttonPosYRow1 = 441;
        constexpr uint32_t buttonPosYRow2 = 479;
        constexpr uint32_t buttonPosYRow3 = 517;

        constexpr uint32_t scanListPosY = 255;
        constexpr uint32_t scanListW    = 400;
        constexpr uint32_t scanListH    = 185;
    } // namespace main_window

    namespace scan_mode_window
    {
        constexpr uint32_t commonXPos       = 40;
        constexpr uint32_t commonYPos       = 110;
        constexpr uint32_t commonSpacerH    = 10;
        constexpr uint32_t commonDefaultPos = 0;

        constexpr uint32_t buttonW = 420;
        constexpr uint32_t buttonH = 33;

        constexpr uint32_t reusultLabelPosY = 500;

        constexpr uint32_t commonFocusPen   = 3;
        constexpr uint32_t commonNoFocusPen = 1;
    } // namespace scan_mode_window

    namespace algo_window
    {

        constexpr uint32_t selectedAntennaYPos = 110;
        constexpr uint32_t selectedAntennaH    = 50;
        constexpr uint32_t infoYPos            = 160;
        constexpr uint32_t culumnLabelYPos     = 199;
        constexpr uint32_t commonH             = 33;

        constexpr uint32_t leftColumnXPos  = 20;
        constexpr uint32_t rightColumnXPos = 250;
        constexpr uint32_t boxW            = 210;
        constexpr uint32_t boxH            = 300;
        constexpr uint32_t boxYPos         = 232;

        constexpr uint32_t commonW = 440;

        constexpr uint32_t commonNoFocusPen = 0;

    } // namespace algo_window
} // namespace antenna

#endif /* MODULE_APPS_APPLICATION_ANTENNA_ANTENNAAPPSTYLE_HPP_ */
