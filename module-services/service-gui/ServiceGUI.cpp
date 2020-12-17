// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceGUI.hpp"
#include "WorkerGUI.hpp"

#include "messages/GUIMessage.hpp"
#include "messages/DrawMessage.hpp"
#include "messages/RenderingFinished.hpp"
#include "messages/GUIDisplayReady.hpp"

#include <DrawCommand.hpp>
#include <FontManager.hpp>
#include <MessageType.hpp>
#include <gui/core/Context.hpp>
#include <gui/core/ImageManager.hpp>
#include <log/log.hpp>
#include <projdefs.h>
#include <service-appmgr/Controller.hpp>
#include <service-eink/messages/ImageMessage.hpp>
#include <service-eink/messages/EinkMessage.hpp>
#include <service-eink/messages/StateRequest.hpp>
#include <Service/Bus.hpp>
#include <Service/Worker.hpp>
#include <SystemManager/SystemManager.hpp>
#include <service-eink/Common.hpp>

#include <task.h>

extern "C"
{
#include <FreeRTOS.h>
#include <semphr.h>
}

#include <cstddef>
#include <list>
#include <memory>
#include <utility>
#include <purefs/filesystem_paths.hpp>

namespace sgui
{
    ServiceGUI::ServiceGUI(const std::string &name, std::string parent, uint32_t screenWidth, uint32_t screenHeight)
        : sys::Service(name, parent, 4096, sys::ServicePriority::Idle), renderContext{nullptr},
          transferContext{nullptr}, renderFrameCounter{1}, transferedFrameCounter{0}, screenWidth{screenWidth},
          screenHeight{screenHeight}, semCommands{NULL}
    {

        LOG_INFO("[ServiceGUI] Initializing");

        renderContext   = new gui::Context(screenWidth, screenHeight);
        transferContext = new gui::Context(screenWidth, screenHeight);

        gui::FontManager &fontManager = gui::FontManager::getInstance();
        fontManager.init(purefs::dir::getCurrentOSPath() / "assets");

        gui::ImageManager &imageManager = gui::ImageManager::getInstance();
        imageManager.init(purefs::dir::getCurrentOSPath() / "assets");

        connect(typeid(sgui::DrawMessage),
                [&](sys::Message *request) -> sys::MessagePointer { return handleDrawMessage(request); });

        connect(typeid(service::gui::RenderingFinished),
                [&](sys::Message *request) -> sys::MessagePointer { return handleGUIRenderingFinished(request); });

        connect(typeid(service::gui::GUIDisplayReady),
                [&](sys::Message *request) -> sys::MessagePointer { return handleGUIDisplayReady(request); });
    }

    ServiceGUI::~ServiceGUI()
    {
        LOG_INFO("[ServiceGUI] Cleaning resources");
        if (renderContext)
            delete renderContext;
        if (transferContext)
            delete transferContext;
    }

    void ServiceGUI::sendBuffer()
    {
        transferContext->insert(0, 0, renderContext);

        auto msg =
            std::make_shared<service::eink::ImageMessage>(0,
                                                          0,
                                                          transferContext->getW(),
                                                          transferContext->getH(),
                                                          (mode == gui::RefreshModes::GUI_REFRESH_DEEP ? true : false),
                                                          transferContext->getData(),
                                                          suspendInProgress,
                                                          shutdownInProgress);
        einkReady = false;
        auto ret  = sys::Bus::SendUnicast(msg, service::name::eink, this, 2000);
        if (ret.first == sys::ReturnCodes::Success) {
            transferedFrameCounter = renderFrameCounter;
        }
        mode = gui::RefreshModes::GUI_REFRESH_FAST;
    }

    void ServiceGUI::sendToRender()
    {
        rendering = true;
        worker->send(static_cast<uint32_t>(WorkerGUICommands::Render), NULL);
    }

    sys::MessagePointer ServiceGUI::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        return std::make_shared<sys::ResponseMessage>();
    }

    sys::ReturnCodes ServiceGUI::InitHandler()
    {
        semCommands = xSemaphoreCreateBinary();
        if (semCommands == NULL) {
            LOG_FATAL("Failed to create commands semaphore.");
            return sys::ReturnCodes::Failure;
        }
        xSemaphoreGive(semCommands);

        worker = std::make_unique<WorkerGUI>(this);
        std::list<sys::WorkerQueueInfo> list;
        worker->init(list);
        worker->run();

        if (einkReady == false) {
            requestSent = true;
            sys::Bus::SendUnicast(std::make_shared<service::eink::StateRequest>(), service::name::eink, this);
        }
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ServiceGUI::DeinitHandler()
    {

        if (semCommands != NULL)
            vSemaphoreDelete(semCommands);
        semCommands = NULL;

        worker->stop();
        worker->join();
        worker->deinit();

        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ServiceGUI::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
    {
        LOG_FATAL("[ServiceGUI] PowerModeHandler: %s", c_str(mode));

        switch (mode) {
        case sys::ServicePowerMode ::Active:
            break;
        case sys::ServicePowerMode ::SuspendToRAM:
        case sys::ServicePowerMode ::SuspendToNVM:
            break;
        }

        return sys::ReturnCodes::Success;
    }

    sys::MessagePointer ServiceGUI::handleDrawMessage(sys::Message *message)
    {
        auto dmsg = static_cast<sgui::DrawMessage *>(message);
        if (!dmsg->commands.empty()) {

            if (!suspendInProgress) {

                if (dmsg->isType(sgui::DrawMessage::Type::SHUTDOWN)) {
                    LOG_WARN("Shutdown - received shutdown draw commands");
                    shutdownInProgress = true;
                }

                if (dmsg->isType(sgui::DrawMessage::Type::SUSPEND)) {
                    LOG_WARN("Suspended - received suspend draw commands");
                    suspendInProgress = true;
                }

                if (dmsg->mode == gui::RefreshModes::GUI_REFRESH_DEEP) {
                    mode = dmsg->mode;
                }

                if (xSemaphoreTake(semCommands, pdMS_TO_TICKS(1000)) == pdTRUE) {
                    commands = std::move(dmsg->commands);
                    xSemaphoreGive(semCommands);
                }
                else {
                    LOG_ERROR("Failed to acquire semaphore");
                }

                if (!rendering) {
                    sendToRender();
                }
            }
            else {
                LOG_WARN("Suspended - ignoring draw commands");
            }
        }
        return nullptr;
    }

    sys::MessagePointer ServiceGUI::handleGUIRenderingFinished(sys::Message *message)
    {
        rendering = false;
        renderFrameCounter++;

        if (einkReady) {
            sendBuffer();
        }
        else if (!requestSent) {
            requestSent = true;
            sys::Bus::SendUnicast(std::make_shared<service::eink::StateRequest>(), service::name::eink, this);
        }
        return nullptr;
    }

    sys::MessagePointer ServiceGUI::handleGUIDisplayReady(sys::Message *message)
    {
        auto msg    = static_cast<service::gui::GUIDisplayReady *>(message);
        einkReady   = true;
        requestSent = false;

        if (msg->getShutdownInProgress()) {
            einkReady         = false;
            suspendInProgress = false;
            LOG_DEBUG("last rendering before shutdown finished.");

            sys::SystemManager::CloseSystem(this);
        }

        if (msg->getSuspendInProgress()) {
            einkReady         = false;
            suspendInProgress = false;
            LOG_DEBUG("last rendering before suspend is finished.");

            app::manager::Controller::changePowerSaveMode(this);
        }
        if ((renderFrameCounter != transferedFrameCounter) && (!rendering)) {
            sendBuffer();
        }

        if (commands.empty() == false) {
            sendToRender();
        }
        return nullptr;
    }

} /* namespace sgui */
