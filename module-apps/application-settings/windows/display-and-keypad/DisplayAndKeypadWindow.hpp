#pragma once

#include <functional>
#include <string>

#include "AppWindow.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"
#include <memory>
#include <BoxLayout.hpp>
#include "OptionWindow.hpp"

namespace gui
{
    std::list<gui::Option> displayAndKeypadOptions(app::Application *app);
} // namespace gui
