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

      protected:
        gui::Size screen = {480, 600};
        // flag that defines whether eink is ready for new frame buffer
        volatile bool einkReady   = false;
        volatile bool requestSent = false;
        volatile bool rendering   = false;

        gui::RefreshModes mode = gui::RefreshModes::GUI_REFRESH_DEEP;

        WorkerGUI *worker;

        /**
         * Flag controls process of redrawing screen when suspend is in progress.
         */
        bool suspendInProgress = false;
        /**
         * Flag controls process of redrawing screen when phone is shutting down.
         */
        bool shutdownInProgress = false;

        void sendBuffer();
        void sendToRender();

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
