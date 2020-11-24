// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common.hpp>
#include <gui/core/Renderer.hpp>
#include <gui/input/Translator.hpp>
#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "source/FrameStack.hpp"

namespace gui
{
    class Context;
    class DrawCommand;
} // namespace gui

namespace sgui
{

    class WorkerGUI;

    class ServiceGUI : public sys::Service
    {
        friend WorkerGUI;
        service::renderer::FrameStack fs;

        enum State
        {
            Initializing,
            AwaitDisplay,
            Processing, // { processing f1 || sending f1 || processing f2 || sending f2 }
            Idle,
        };

      protected:
        gui::Size screen = {480, 600};

        WorkerGUI *worker = nullptr;

        /**
         * Flag controls process of redrawing screen when suspend is in progress.
         */
        bool suspendInProgress = false;
        /**
         * Flag controls process of redrawing screen when phone is shutting down.
         */
        bool shutdownInProgress = false;

        void sendBuffer();

      public:
        ServiceGUI(const std::string &name, std::string parent, gui::Size screenSize);

        sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

        sys::ReturnCodes InitHandler() override;

        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;

      private:
        sys::Message_t handleDrawMessage(sys::Message *message);
        sys::Message_t handleGUIRenderingFinished(sys::Message *message);
        sys::Message_t handleGUIDisplayReady(sys::Message *message);
    };

} /* namespace sgui */
