#pragma once

#include "Application.hpp"
#include "OptionWindow.hpp"
#include <UiCommonActions.hpp>

namespace gui::options
{
    Option callOption(app::Application *app, app::CallOperation callOperation, const ContactRecord &contact);
    Option contactOption(app::Application *app, app::ContactOperation contactOperation, const ContactRecord &contactRec,
                         gui::Arrow arrow = gui::Arrow::Disabled);
} // namespace gui::options
