// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-test/ApplicationTest.hpp"
#include "log/log.hpp"
#include "presenters/TestPresenter.hpp"
#include "windows/TestPopup.hpp"
#include "windows/TestWindow.hpp"

namespace app
{
    ApplicationTest::ApplicationTest(const std::string &name,
                                     const std::string &parent,
                                     StatusIndicators statusIndicators,
                                     StartInBackground startInBackground)
        : Application(name, parent, statusIndicators, startInBackground)
    {
        LOG_INFO("created!");
    }

    ApplicationTest::~ApplicationTest()
    {
        // this destructor is needed for unique_ptr forward declared TestPresenter
    }

    sys::ReturnCodes ApplicationTest::InitHandler()
    {
        LOG_INFO("initializing!");
        // NOTE
        // this is very important! as it:
        // 1. sets state initializing on app
        // 2. initailizes settigns - every app have settings object by default
        const auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            LOG_ERROR("init app failure! %d", int(ret));
            return ret;
        }
        createUserInterface();
        return sys::ReturnCodes::Success;
    }

    sys::MessagePointer ApplicationTest::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        // NOTE
        // If return value here is `nullptr` - then application never responds
        // otherwise it will respond with ResponseMessage
        auto response = sys::MessagePointer{};

        // NOTE in this place we have all common application uses such as:
        // * passing key presses to windows
        // * updading top bar etc
        response = Application::DataReceivedHandler(msgl);
        // NOTE
        // here we would have handleAsyncResponse if this Applicaition would have any `AsyncTask`
        // if (response && (dynamic_cast<sys::ResponseMessage *>(response.get())->retCode == sys::ReturnCodes::Success))
        // {
        //      return response;
        // }
        // return handleAsyncResponse(resp);
        return response;
    }

    void ApplicationTest::createUserInterface()
    {
        // NOTE
        // create presenter first
        presenter = std::make_unique<gui::TestPresenter>(this);
        // NOTE
        // 1. we pass presenter to this application
        // 2. unfortunately any window requires access to app by design - so we pass it too...
        // 3. please note that window is not yet created - it will be created when and if needed
        // 4. please see that window name is set dynamically - this is proper way to set window name - these names are
        // used on windows stack and matters
        windowsFactory.attach(gui::name::window::main_window,
                              [this](ApplicationCommon * /*app*/, const std::string &name) {
                                  return std::make_unique<gui::TestMainWindow>(this, name, *presenter);
                              });

        // This is just a test popup to show on ServiceTest popup request
        windowsFactory.attach(gui::popup::window::test_popup,
                              [this](ApplicationCommon * /*app*/, const std::string &name) {
                                  return std::make_unique<gui::TestPopupWindow>(this, name);
                              });

        // NOTE
        // 1. attach popup
        // 2. then in apps-common/ApplicationCommonPopupBlueprints.cpp register blueprint for this popup
        // 3. if popup window is not attached - by default application will try to switch to this window
        //    but via popup handling
        // 4. popups are called via actions via sending message directly or via call:
        //        app::manager::Controller::sendAction(this, app::manager::actions::ShowPopup,
        //        std::make_unique<PowerOffPopupRequestParams>());
        //    or just directly doing what's done in controler
        //        return std::make_unique<app::manager::ActionRequest>(sender, app::manager::actions::ShowPopup,
        //        std::make_unique<gui::PopupRequestParams>(gui::popup::ID::AppTestPopup));
        attachPopups({gui::popup::ID::AppTestPopup});
    }

    void ApplicationTest::destroyUserInterface()
    {
        // NOTE: it should be empty - as it rather shouldn't be needed
        // but this is a place where i.e. you can set last exit state of this app
    }
} // namespace app
