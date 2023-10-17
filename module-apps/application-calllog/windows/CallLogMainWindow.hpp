// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CalllogModel.hpp"
#include "header/DeleteAction.hpp"

#include <AppWindow.hpp>
#include <ListView.hpp>
#include <Icon.hpp>

namespace gui
{

    class CallLogMainWindow : public AppWindow
    {
        std::shared_ptr<CalllogModel> calllogModel = nullptr;
        gui::ListView *list                        = nullptr;

        gui::VBox *emptyLayout = nullptr;
        bool isEmpty() const;

        header::DeleteAction *deleteAction = nullptr;

      public:
        explicit CallLogMainWindow(app::ApplicationCommon *app);

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(InputEvent const &inputEvent) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      private:
        void buildInterfaceForEmptyState();

        bool onDatabaseMessage(sys::Message *msg) override;
        void onEmptyList();
        void onListFilled();
    };

} /* namespace gui */
