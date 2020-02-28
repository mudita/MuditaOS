#pragma once

#include "Application.hpp"
#include "OptionWindow.hpp"
#include <UiCommonActions.hpp>

namespace app
{
    gui::Option callOption(Application *app, CallOperation callOperation, const ContactRecord &contact);
    gui::Option contactOption(Application *app, ContactOperation contactOperation, const ContactRecord &contact);
} // namespace app
