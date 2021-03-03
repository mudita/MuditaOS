// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef MODULE_APPS_APPLICATION_SETTINGS_WINDOWS_LANGUAGEWINDOW_HPP_
#define MODULE_APPS_APPLICATION_SETTINGS_WINDOWS_LANGUAGEWINDOW_HPP_

#include <string>
#include <functional>

#include "AppWindow.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Window.hpp"
#include "gui/widgets/BottomBar.hpp"

namespace gui
{

    class LanguageWindow : public AppWindow
    {
      protected:
        std::vector<gui::Item *> options;

        gui::Item *addOptionLabel(const std::string &text, std::function<bool(Item &)> activatedCallback);

      public:
        LanguageWindow(app::Application *app);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        utils::LangLoader loader;
        void invalidate() noexcept;
    };
} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_SETTINGS_WINDOWS_LANGUAGEWINDOW_HPP_ */
