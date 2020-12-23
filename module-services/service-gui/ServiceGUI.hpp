// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common.hpp>
#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>

#include "ContextPool.hpp"

#include <cstdint>
#include <memory>
#include <string>

namespace gui
{
    class Context;
    class DrawCommand;
} // namespace gui

namespace service::gui
{
    class WorkerGUI;

    class ServiceGUI : public sys::Service
    {
        friend WorkerGUI;

      public:
        explicit ServiceGUI(const std::string &name, std::string parent = {});
        ~ServiceGUI() noexcept override;

        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;
        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;

      private:
        enum class State
        {
            NotInitialised,
            Running,
            Suspended
        };
        void setState(State state) noexcept;
        bool isInState(State state) const noexcept;

        static void initAssetManagers();
        void registerMessageHandlers();
        void notifyRenderer(std::list<std::unique_ptr<::gui::DrawCommand>> &&commands, ::gui::RefreshModes refreshMode);

        sys::MessagePointer handleDrawMessage(sys::Message *message);
        sys::MessagePointer handleGUIRenderingFinished(sys::Message *message);
        sys::MessagePointer handleEinkReady(sys::Message *message);

        std::unique_ptr<ContextPool> contextPool;
        std::unique_ptr<WorkerGUI> worker;
        State currentState;
    };
} // namespace service::gui
