// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CalllogModel.hpp"

#include <AppWindow.hpp>
#include <ListView.hpp>
#include <Icon.hpp>

namespace gui
{

    class CallLogMainWindow : public AppWindow
    {
        std::shared_ptr<CalllogModel> calllogModel = nullptr;
        gui::ListView *list                        = nullptr;
        gui::Icon *emptyListIcon                   = nullptr;

      public:
        explicit CallLogMainWindow(app::Application *app);

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        bool onDatabaseMessage(sys::Message *msg) override;
        void onEmptyList();
        void onListFilled();
    };

} /* namespace gui */
