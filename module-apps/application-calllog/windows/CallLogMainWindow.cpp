// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallLogMainWindow.hpp"
#include "application-calllog/data/CallLogInternals.hpp"
#include "application-calllog/ApplicationCallLog.hpp"
#include "application-calllog/widgets/CalllogItem.hpp"

#include <application-call/ApplicationCall.hpp>
#include <service-appmgr/model/ApplicationManager.hpp>
#include <service-db/DBCalllogMessage.hpp>
#include <i18n/i18n.hpp>
#include <Label.hpp>
#include <Margins.hpp>
#include <Style.hpp>
#include <InputEvent.hpp>

#include <cassert>
#include <functional>
#include <memory>
#include <module-services/service-db/service-db/DBNotificationMessage.hpp>

using namespace style;
using namespace callLogStyle;

namespace gui
{

    CallLogMainWindow::CallLogMainWindow(app::Application *app)
        : AppWindow(app, calllog::settings::MainWindowStr), calllogModel{std::make_shared<CalllogModel>(app)}
    {

        buildInterface();
    }

    void CallLogMainWindow::rebuild()
    {
        if (list == nullptr) {
            return;
        }
        list->rebuildList(style::listview::RebuildType::InPlace);
    }

    void CallLogMainWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::localize.get("app_calllog_title_main"));

        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get(style::strings::common::call));
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::open));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        list = new gui::ListView(this,
                                 mainWindow::x,
                                 mainWindow::y,
                                 mainWindow::w,
                                 mainWindow::h,
                                 calllogModel,
                                 style::listview::ScrollBarType::Fixed);

        setFocusItem(list);
    }

    void CallLogMainWindow::destroyInterface()
    {
        erase();
    }

    void CallLogMainWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (mode == ShowMode::GUI_SHOW_INIT) {
            list->rebuildList();
        }
        auto app = dynamic_cast<app::ApplicationCallLog *>(application);
        assert(app != nullptr);
        app->setAllEntriesRead();
    }

    bool CallLogMainWindow::onDatabaseMessage(sys::Message *msg)
    {
        auto notification = dynamic_cast<db::NotificationMessage *>(msg);
        if (notification != nullptr) {
            if (notification->interface == db::Interface::Name::Calllog && notification->dataModified()) {
                rebuild();
                return true;
            }
        }
        return false;
    }
} /* namespace gui */
