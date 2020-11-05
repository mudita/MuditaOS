// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "GUIMessage.hpp"

namespace service::renderer
{
    class RenderingFinished : public sgui::GUIMessage
    {
        bool success = false;

      public:
        RenderingFinished(bool success) : success(success)
        {}

        [[nodiscard]] bool isSuccess() const noexcept
        {
            return success;
        }

        operator std::string() const override
        {
            return std::string("{ ") + (success ? "Success" : "Fail") + "}";
        }
    };
} // namespace service::renderer
