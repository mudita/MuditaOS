#pragma once

#include <functional>
#include <string>

#include "AppWindow.hpp"
#include "ListView.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"
#include <gui/widgets/BoxLayout.hpp>

namespace gui
{

    namespace name
    {
        namespace window
        {
            inline std::string thread_view = "ThreadViewWindow";
        };
    }; // namespace name

    class ThreadViewWindow : public AppWindow
    {
      private:
        gui::VBox *body = nullptr;
        void cleanMessages();
        void addMessages(uint32_t thread_id);

      public:
        ThreadViewWindow(app::Application *app);
        virtual ~ThreadViewWindow();

        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        bool onDatabaseMessage(sys::Message *msgl) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */
