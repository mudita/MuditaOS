// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DrawMessage.hpp"
#include "GUIMessage.hpp"
#include <magic_enum.hpp>
#include <Common.hpp>
#include <DrawCommand.hpp>
#include <json/json11.hpp>

namespace sgui
{

    DrawMessage::DrawMessage(std::list<gui::Command> commands, gui::RefreshModes mode)
        : GUIMessage(), mode(mode), commands(std::move(commands))
    {
    }

    DrawMessage::operator std::string() const
    {
        json11::Json::object vals;
        vals["Type"] = std::string(magic_enum::enum_name(type));
        vals["Mode"] = std::string(magic_enum::enum_name(mode));
        vals["size"] = std::to_string(commands.size());
        return json11::Json(vals).dump();
    }
} /* namespace sgui */
