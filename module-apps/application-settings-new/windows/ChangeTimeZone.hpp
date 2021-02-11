#pragma once

#include "BaseSettingsWindow.hpp"

namespace gui
{

    class ChangeTimeZone : public BaseSettingsWindow
    {
      public:
        explicit ChangeTimeZone(app::Application *app);
        auto buildOptionsList() -> std::list<Option> override;
    };
} // namespace gui
