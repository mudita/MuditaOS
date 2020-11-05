// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "GUIMessage.hpp"
#include <gui/Common.hpp>
#include <Service/Message.hpp>

#include <list>
#include <memory>

#include "Service/Message.hpp"
#include "GUIMessage.hpp"
#include "gui/Common.hpp"
#include <service-gui/source/DrawData.hpp>
#include <utility>

namespace sgui
{

    class DrawMessage : public GUIMessage
    {
        DrawData data;

      public:
        enum class Type
        {
            NORMAL,
            SUSPEND,
            SHUTDOWN
        } type = Type::NORMAL;

        DrawMessage(DrawData data) : data(std::move(data))
        {}

        void setCommandType(Type type) noexcept
        {
            this->type = type;
        }

        bool isType(Type type) const noexcept
        {
            return this->type == type;
        }

        [[nodiscard]] DrawData takeDrawData()
        {
            return std::move(data);
        }

        operator std::string() const override;
    };

} // namespace sgui
