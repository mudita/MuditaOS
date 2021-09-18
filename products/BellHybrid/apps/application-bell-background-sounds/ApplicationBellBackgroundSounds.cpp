// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellBackgroundSounds.hpp"
#include "presenter/BGSoundsMainWindowPresenter.hpp"
#include "presenter/BGSoundsTimerSelectPresenter.hpp"
#include "presenter/BGSoundsProgressPresenter.hpp"
#include "windows/BGSoundsMainWindow.hpp"
#include "windows/BGSoundsPausedWindow.hpp"
#include "windows/BGSoundsProgressWindow.hpp"
#include "windows/BGSoundsTimerSelectWindow.hpp"

namespace app
{
    ApplicationBellBackgroundSounds::ApplicationBellBackgroundSounds(std::string name,
                                                                     std::string parent,
                                                                     StatusIndicators statusIndicators,
                                                                     StartInBackground startInBackground)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground)
    {}

    sys::ReturnCodes ApplicationBellBackgroundSounds::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }
        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellBackgroundSounds::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](ApplicationCommon *app, const std::string &name) {
            auto presenter = std::make_unique<bgSounds::BGSoundsMainWindowPresenter>();
            return std::make_unique<gui::BGSoundsMainWindow>(app, std::move(presenter));
        });
        windowsFactory.attach(
            gui::window::name::bgSoundsTimerSelect, [this](ApplicationCommon *app, const std::string &name) {
                auto presenter = std::make_unique<bgSounds::BGSoundsTimerSelectPresenter>(settings.get());
                return std::make_unique<gui::BGSoundsTimerSelectWindow>(app, std::move(presenter));
            });
        windowsFactory.attach(
            gui::window::name::bgSoundsProgress, [this](ApplicationCommon *app, const std::string &name) {
                auto presenter = std::make_unique<bgSounds::BGSoundsProgressPresenter>(app, settings.get());
                return std::make_unique<gui::BGSoundsProgressWindow>(app, std::move(presenter));
            });
        windowsFactory.attach(gui::window::name::bgSoundsPaused, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::BGSoundsPausedWindow>(app);
        });
    }

    sys::MessagePointer ApplicationBellBackgroundSounds::DataReceivedHandler(sys::DataMessage *msgl,
                                                                             sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        if (auto response = dynamic_cast<sys::ResponseMessage *>(retMsg.get());
            response != nullptr && response->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }
        return handleAsyncResponse(resp);
    }
} // namespace app
