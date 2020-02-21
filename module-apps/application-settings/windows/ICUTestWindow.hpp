#pragma once

#include <AppWindow.hpp>
#include <gui/widgets/BoxLayout.hpp>
#include <gui/widgets/Label.hpp>
#include <gui/widgets/Text.hpp>

#include <string>

namespace gui
{
    class ICUTestWindow : public AppWindow
    {
      public:
        ICUTestWindow(app::Application *parent, std::string window_name);
        virtual ~ICUTestWindow();

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        void updateDate();
        VBox *mainBox;
        gui::Label *plShort;
        gui::Text *dayText;
        gui::Text *monthText;
        gui::Text *yearText;
        gui::Text *localeText;
    };

} // namespace gui
