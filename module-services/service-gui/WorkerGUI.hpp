// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ServiceGUI.hpp"

#include <core/Context.hpp>
#include <core/DrawCommand.hpp>
#include <gui/core/Renderer.hpp>
#include <Service/Worker.hpp>

#include <cstdint>

namespace sgui
{

    class ServiceGUI;

    enum class WorkerGUICommands
    {
        Finish,
        Render,
        //	RenderSuspend
    };

    /*
     *
     */
    class WorkerGUI : public sys::Worker
    {
        gui::Renderer renderer;

      public:
        WorkerGUI(ServiceGUI *service);

        /**
         * virtual method responsible for finishing the worker and handling rendering commands
         */
        bool handleMessage(uint32_t queueID) override;
    };

} /* namespace sgui */
