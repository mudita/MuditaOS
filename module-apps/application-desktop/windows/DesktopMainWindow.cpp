/*
 * @file DesktopMainWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 19 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <memory>
#include <functional>

#include "../ApplicationDesktop.hpp"
#include "../data/LockPhoneData.hpp"
#include "Alignment.hpp"
#include "Common.hpp"
#include "DesktopMainWindow.hpp"
#include "application-calllog/ApplicationCallLog.hpp"
#include "application-messages/ApplicationMessages.hpp"
#include "gui/widgets/Image.hpp"
#include "service-appmgr/ApplicationManager.hpp"

#include "application-call/ApplicationCall.hpp"
#include "application-call/data/CallSwitchData.hpp"
#include <UiCommonActions.hpp>

#include "Common.hpp"
#include "i18/i18.hpp"
#include <Span.hpp>
#include <Style.hpp>
#include <application-settings/ApplicationSettings.hpp>
#include <cassert>
#include <service-appmgr/ApplicationManager.hpp>
#include <time/time_conversion.hpp>

namespace gui
{

    void DesktopMainWindow::buildInterface()
    {
        AppWindow::buildInterface();
    }

    void DesktopMainWindow::destroyInterface()
    {
    }

    DesktopMainWindow::DesktopMainWindow(app::Application *app) : AppWindow(app, app::window::name::desktop_main_window)
    {
        buildInterface();
    }

    DesktopMainWindow::~DesktopMainWindow()
    {
    }

    void DesktopMainWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
    }

    bool DesktopMainWindow::onInput(const InputEvent &inputEvent)
    {
        return AppWindow::onInput(inputEvent);
    }

    void DesktopMainWindow::rebuild()
    {
    }

} /* namespace gui */
