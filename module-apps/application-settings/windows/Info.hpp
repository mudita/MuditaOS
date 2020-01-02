#pragma once

#include <functional>
#include <string>

#include "AppWindow.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"
#include <BoxLayout.hpp>
#include <memory>

namespace gui
{

    namespace window
    {
        inline const std::string hw_info = "hw_info";
    };

    class Info : public AppWindow
    {
      protected:
        VBox *box;

        gui::Item *addOptionLabel(const std::string &text, std::function<bool(Item &)> activatedCallback);

      public:
        Info(app::Application *app);
        virtual ~Info();

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        void set_navigation();
    };
} // namespace gui
