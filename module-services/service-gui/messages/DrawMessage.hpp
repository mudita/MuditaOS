/*
 * @file DrawMessage.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 22 maj 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_SERVICES_SERVICE_GUI_DRAWMESSAGE_HPP_
#define MODULE_SERVICES_SERVICE_GUI_DRAWMESSAGE_HPP_

#include <list>
#include <memory>

#include "Service/Message.hpp"
#include "core/DrawCommand.hpp"
#include "GUIMessage.hpp"
#include "gui/Common.hpp"

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
