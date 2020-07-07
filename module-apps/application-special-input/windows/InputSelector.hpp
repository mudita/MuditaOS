#pragma once

#include <functional>
#include <string>

#include "AppWindow.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"
#include <GridLayout.hpp>
#include <memory>

namespace gui
{

    class UiCharSelector : public AppWindow
    {
      protected:
        GridLayout *box;

      public:
        UiCharSelector(app::Application *app);
        virtual ~UiCharSelector() = default;

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        Item *buildNewline();
        /// add Activation action to item in character selection
        /// on click on item relevant utf value should be send to requesting app & window
        /// i.e. requesting `ę` should send string with it's code
        void decorateActionActivated(Item *it, const std::string &str);
    };

}; // namespace gui
