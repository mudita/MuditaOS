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

      public:
        Info(app::Application *app);

        // virtual methods
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        void addAlignedLabelWithValue(BoxLayout *layout, const std::string &labelText, const std::string &valueText);
    };
} // namespace gui
