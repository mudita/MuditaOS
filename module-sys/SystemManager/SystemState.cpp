// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SystemState.hpp"

namespace sys
{
    SystemState::SystemState()
        : screenState(ScreenState::Lock), audioState(AudioState::Unactive),
          applicationsState(ApplicationState::Unactive), rtcState(RtcState::Outdated),
          cellularState(CellularState::Unready), chargerState(ChargerState::Unplugged)
    {}

    SystemState::~SystemState()
    {}

    void SystemState::SetScreenState(ScreenState state)
    {
        screenState = state;
    }

    void SystemState::SetAudioState(AudioState state)
    {
        audioState = state;
    }
    void SystemState::SetFocusOnApplication(ApplicationState state)
    {
        applicationsState = state;
    }

    void SystemState::SetRtcState(RtcState state)
    {
        rtcState = state;
    }

    void SystemState::SetCellularState(CellularState state)
    {
        cellularState = state;
    }

    void SystemState::SetChargerState(ChargerState state)
    {
        chargerState = state;
    }

    bool SystemState::LowPowerPermission()
    {
        return (screenState == ScreenState::Lock && audioState == AudioState::Unactive &&
                applicationsState == ApplicationState::Unactive && rtcState == RtcState::Updated &&
                cellularState == CellularState::Ready && chargerState == ChargerState::Unplugged);
    }

} // namespace sys
