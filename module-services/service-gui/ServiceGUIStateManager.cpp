// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceGUIStateManager.hpp"

namespace service::gui
{
    bool ServiceGUIStateManager::isInState(DisplayingState state)
    {
        return displayingState == state;
    }
    bool ServiceGUIStateManager::isInState(RenderingState state)
    {
        return renderingState == state;
    }
    bool ServiceGUIStateManager::isInState(ServiceGUIState state)
    {
        return serviceState == state;
    }

    void ServiceGUIStateManager::setState(DisplayingState state)
    {
        displayingState = state;
    }
    void ServiceGUIStateManager::setState(RenderingState state)
    {
        renderingState = state;
    }
    void ServiceGUIStateManager::setState(ServiceGUIState state)
    {
        if (serviceState == ServiceGUIState::Closed) {
            return;
        }
        serviceState = state;
    }
} // namespace service::gui
