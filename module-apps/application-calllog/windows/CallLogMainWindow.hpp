// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <functional>

#include "AppWindow.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Window.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/ListView.hpp"

#include "CalllogModel.hpp"

namespace gui
{

    class CallLogMainWindow : public AppWindow
    {
        std::shared_ptr<CalllogModel> calllogModel = nullptr;
        gui::ListView *list                        = nullptr;

      public:
        explicit CallLogMainWindow(app::Application *app);

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        bool onDatabaseMessage(sys::Message *msg) override;
    };

} /* namespace gui */
