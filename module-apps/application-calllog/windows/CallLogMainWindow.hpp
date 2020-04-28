/*
 * @file CallLogMainWindow.hpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 19.09.2019
 * @brief Application Call Log Main Window
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#pragma once

#include <string>
#include <functional>

#include "AppWindow.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Window.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"
#include "gui/widgets/ListView.hpp"

#include "../CalllogModel.hpp"

namespace gui
{

    class CallLogMainWindow : public AppWindow
    {
        CalllogModel *calllogModel = nullptr;
        gui::ListView *list        = nullptr;

      public:
        CallLogMainWindow(app::Application *app);
        virtual ~CallLogMainWindow();

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        bool onDatabaseMessage(sys::Message *msg) override;
    };

} /* namespace gui */
