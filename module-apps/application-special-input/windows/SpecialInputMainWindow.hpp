#pragma once

#include <functional>
#include <string>

#include "AppWindow.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"
#include <GridLayout.hpp>
#include <memory>
#include <module-gui/gui/widgets/ListView.hpp>
#include <module-apps/application-special-input/models/SpecialInputModel.hpp>

namespace gui
{

    class SpecialInputMainWindow : public AppWindow
    {
      protected:
        ListView *charList                             = nullptr;
        std::shared_ptr<SpecialInputModel> model       = nullptr;
        specialInputStyle::CharactersType actualWindow = specialInputStyle::CharactersType::SpecialCharacters;
        void switchPage();

      public:
        SpecialInputMainWindow(app::Application *app);
        virtual ~SpecialInputMainWindow() = default;

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

}; // namespace gui
