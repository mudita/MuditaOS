// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef MODULE_SERVICES_SERVICE_GUI_DRAWMESSAGE_HPP_
#define MODULE_SERVICES_SERVICE_GUI_DRAWMESSAGE_HPP_

#include <list>   // for list
#include <memory> // for unique_ptr

#include "Service/Message.hpp"
#include "core/DrawCommand.hpp"
#include "GUIMessage.hpp" // for GUIMessage
#include "gui/Common.hpp" // for RefreshModes

namespace gui
{
    class DrawCommand;
} // namespace gui

namespace sgui
{

    /*
     *
     */
    class DrawMessage : public GUIMessage
    {
      public:
        enum class DrawCommand
        {
            NORMAL,
            SUSPEND,
            SHUTDOWN
        };

      public:
        gui::RefreshModes mode;
        std::list<std::unique_ptr<gui::DrawCommand>> commands;

        /**
         * flag that informs that this is last rendering before suspending system.
         */
        DrawCommand command = DrawCommand::NORMAL;

        DrawMessage(const std::list<gui::DrawCommand *> &commandsList,
                    gui::RefreshModes mode,
                    DrawCommand cmd = DrawCommand::NORMAL);
        virtual ~DrawMessage();
    };

} /* namespace sgui */

#endif /* MODULE_SERVICES_SERVICE_GUI_DRAWMESSAGE_HPP_ */
