// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <appmgr/IdleHandler.hpp>
#include <appmgr/messages/IdleTimerMessage.hpp>
#include <service-desktop/DesktopMessages.hpp>

#include <service-appmgr/Controller.hpp>
#include <Timers/TimerFactory.hpp>

namespace app::manager
{
    IdleHandler::IdleHandler(sys::Service *serv) : serv{serv}
    {
        idleTimer = sys::TimerFactory::createPeriodicTimer(
            serv, "IdleReturn", idleReturnTimeout, [this](sys::Timer &) { idleTimerCallback(); });
    }

    void IdleHandler::handleStartIdleTimer(sys::Message *request)
    {
        idleTimer.restart(idleReturnTimeout);
    }

    void IdleHandler::handleRestartIdleTimer(sys::Message *request)
    {
        if (idleTimer.isActive()) {
            idleTimer.restart(idleReturnTimeout);
        }
    }

    void IdleHandler::handleStopIdleTimer(sys::Message *request)
    {
        idleTimer.stop();
    }

    void IdleHandler::idleTimerCallback()
    {
        LOG_INFO("User is idle - going back to home screen");
        idleTimer.stop();
        app::manager::Controller::sendAction(serv, app::manager::actions::Home);
    }

    //    void IdleHandler::handleFileTransferMessage(sys::Message *request)
    //    {
    //        using FileTransferAction = sdesktop::fileTransfer::FileTransferAction;
    //        auto message             = static_cast<sdesktop::fileTransfer::FileTransferMessage *>(request);
    //        idleTimer.stop();
    //
    //        const auto action = message->getCurrentFileTransferAction();
    //
    //        LOG_ERROR("File transfer: %s, %d",
    //                  (static_cast<int>(action) == 0 ? "Started" : "Finished"),
    //                  static_cast<int>(action));
    //
    //        switch (action) {
    //
    //        case FileTransferAction::Started: {
    //            app::manager::Controller::sendAction(serv, app::manager::actions::FileTransferStarted);
    //            break;
    //        }
    //        case FileTransferAction::Finished: {
    //            app::manager::Controller::sendAction(serv, app::manager::actions::FileTransferFinished);
    //            break;
    //        }
    //        default: {
    //            LOG_ERROR("INVALID");
    //            break;
    //        }
    //        }
    //    }
} // namespace app::manager
