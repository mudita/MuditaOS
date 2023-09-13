// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
namespace service::gui
{
    enum class RenderingState
    {
        Idle,
        Rendering
    };

    enum class DisplayingState
    {
        Idle,
        Displaying
    };

    enum class ServiceGUIState
    {
        Initializing,
        Running,
        Closing,
        Closed
    };

    class ServiceGUIStateManager
    {
      public:
        ServiceGUIStateManager()  = default;
        ~ServiceGUIStateManager() = default;
        bool isInState(DisplayingState state);
        bool isInState(RenderingState state);
        bool isInState(ServiceGUIState state);

        void setState(DisplayingState state);
        void setState(RenderingState state);
        void setState(ServiceGUIState state);

      private:
        DisplayingState displayingState{DisplayingState::Idle};
        RenderingState renderingState{RenderingState::Idle};
        ServiceGUIState serviceState{ServiceGUIState::Initializing};
    };
} // namespace service::gui
