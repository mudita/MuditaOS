// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * SystemState.hpp
 *
 *  Created on: Nov 10, 2020
 *      Author: Maciej-Mudita
 */

#ifndef SYSTEMMANAGER_SYSTEMSTATE_HPP_
#define SYSTEMMANAGER_SYSTEMSTATE_HPP_

namespace sys
{
    class SystemState
    {
      public:
        enum class ScreenState
        {
            Lock,
            Unlock

        };
        enum class AudioState
        {
            Active,
            Unactive
        };
        enum class ApplicationState
        {
            Active,
            Unactive
        };
        enum class CellularState
        {
            Ready,
            Unready
        };
        enum class RtcState
        {
            Updated,
            Outdated
        };
        enum class ChargerState
        {
            Plugged,
            Unplugged
        };

        SystemState();
        ~SystemState();

        void SetScreenState(ScreenState state);
        void SetAudioState(AudioState state);
        void SetFocusOnApplication(ApplicationState state);
        void SetRtcState(RtcState state);
        void SetCellularState(CellularState state);
        void SetChargerState(ChargerState state);
        bool LowPowerPermission();

      private:
        ScreenState screenState;
        AudioState audioState;
        ApplicationState applicationsState;
        RtcState rtcState;
        CellularState cellularState;
        ChargerState chargerState;
    };

} // namespace sys

#endif /* SYSTEMMANAGER_SYSTEMSTATE_HPP_ */
