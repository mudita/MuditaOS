// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"
#include "OptionWindow.hpp"
#include <UiCommonActions.hpp>

namespace gui::options
{
    Option call(app::Application *app, app::CallOperation callOperation, const ContactRecord &contact);
    Option contact(app::Application *app,
                   app::ContactOperation contactOperation,
                   const ContactRecord &contactRec,
                   gui::Arrow arrow = gui::Arrow::Disabled);
} // namespace gui::options
