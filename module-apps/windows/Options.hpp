// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-db/Interface/ContactRecord.hpp>
#include "Application.hpp"
#include "OptionWindow.hpp"

namespace gui::options
{
    enum class ContactOperation
    {
        Add,
        Details,
        Edit
    };

    Option call(app::Application *app, const ContactRecord &contact);
    Option contact(app::Application *app,
                   ContactOperation contactOperation,
                   const ContactRecord &contactRec,
                   gui::Arrow arrow = gui::Arrow::Disabled);
} // namespace gui::options
