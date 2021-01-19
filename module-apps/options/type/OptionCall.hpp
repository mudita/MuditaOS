// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "OptionSimple.hpp"
#include "Application.hpp"

#include <module-db/Interface/ContactRecord.hpp>

namespace gui::option
{
    class Call : public Simple
    {
      public:
        Call(app::Application *app, const ContactRecord &contact);
    };
}; // namespace gui::option
