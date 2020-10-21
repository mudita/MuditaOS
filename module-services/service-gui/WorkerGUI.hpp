// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef MODULE_SERVICES_SERVICE_GUI_WORKERGUI_HPP_
#define MODULE_SERVICES_SERVICE_GUI_WORKERGUI_HPP_

#include "ServiceGUI.hpp"

#include <stdint.h> // for uint32_t

#include "Service/Worker.hpp" // for Worker
#include "core/Context.hpp"
#include "core/DrawCommand.hpp"
#include "gui/core/Renderer.hpp" // for Renderer
#include "messages/GUIMessage.hpp"

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
        // object responsible for rendering images to context
        gui::Renderer renderer;

      public:
        WorkerGUI(ServiceGUI *service);

        /**
         * virtual method responsible for finishing the worker and handling rendering commands
         */
        bool handleMessage(uint32_t queueID) override;
    };

} /* namespace sgui */

#endif /* MODULE_SERVICES_SERVICE_GUI_WORKERGUI_HPP_ */
