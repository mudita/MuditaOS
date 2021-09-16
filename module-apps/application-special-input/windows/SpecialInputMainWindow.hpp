// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SpecialInputModel.hpp"
#include <gui/widgets/Image.hpp>
#include <gui/widgets/Label.hpp>
#include <gui/widgets/Window.hpp>
#include <AppWindow.hpp>
#include <GridLayout.hpp>
#include <ListView.hpp>
#include <memory>
#include <functional>
#include <string>

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
        SpecialInputMainWindow(app::ApplicationCommon *app);
        virtual ~SpecialInputMainWindow() = default;

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

}; // namespace gui
