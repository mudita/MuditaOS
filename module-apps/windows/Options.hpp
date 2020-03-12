#pragma once

#include "Application.hpp"
#include "OptionWindow.hpp"
#include <UiCommonActions.hpp>

namespace gui::options
{
    Option call(app::Application *app, app::CallOperation callOperation, const ContactRecord &contact);
    Option contact(app::Application *app, app::ContactOperation contactOperation, const ContactRecord &contactRec, gui::Arrow arrow = gui::Arrow::Disabled);
} // namespace gui::options
