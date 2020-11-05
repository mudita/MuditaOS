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

#include <cstddef>
#include <list>
#include <memory>
#include <utility>

namespace sgui
{

    using namespace service::renderer;

    ServiceGUI::ServiceGUI(const std::string &name, std::string parent, gui::Size screenSize)
        : sys::Service(name, parent, 4096, sys::ServicePriority::Idle), fs(screenSize), worker{nullptr}
    {

        LOG_INFO("[ServiceGUI] Initializing");

        gui::FontManager &fontManager = gui::FontManager::getInstance();
        fontManager.init("assets");

        gui::ImageManager &imageManager = gui::ImageManager::getInstance();
        imageManager.init("assets");

        connect(typeid(sgui::DrawMessage), [&](sys::DataMessage *request, sys::ResponseMessage *) -> sys::Message_t {
            return handleDrawMessage(request);
        });

        connect(typeid(RenderingFinished), [&](sys::DataMessage *request, sys::ResponseMessage *) -> sys::Message_t {
            return handleGUIRenderingFinished(request);
        });

        connect(typeid(GUIDisplayReady), [&](sys::DataMessage *request, sys::ResponseMessage *) -> sys::Message_t {
            return handleGUIDisplayReady(request);
        });
    }

    void ServiceGUI::sendBuffer()
    {
        service::eink::ImageData data;
        if (not fs.takeLastProcessedFrame(data)) {
            LOG_ERROR("cant take context to send to eink");
        }

        auto msg =
            std::make_shared<service::eink::ImageMessage>(std::move(data), suspendInProgress, shutdownInProgress);

        einkReady = false;
        sys::Bus::SendUnicast(msg, service::name::eink, this);
        // TODO handle this ... this was blocking
        // if (ret.first == sys::ReturnCodes::Success) {
        //     transferedFrameCounter = renderFrameCounter;
        // }
        mode = gui::RefreshModes::GUI_REFRESH_FAST;
    }

    void ServiceGUI::sendToRender()
    {
        rendering = true;
        worker->send(static_cast<uint32_t>(WorkerGUICommands::Render), NULL);
    }

    sys::Message_t ServiceGUI::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        return std::make_shared<sys::ResponseMessage>();
    }

    sys::ReturnCodes ServiceGUI::InitHandler()
    {
        worker = new WorkerGUI(this);
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

    sys::Message_t ServiceGUI::handleDrawMessage(sys::Message *message)
    {
        auto dmsg = static_cast<sgui::DrawMessage *>(message);
        auto data = dmsg->takeDrawData();
        if (data.getCommands().empty()) {

            if (!suspendInProgress) {

                if (dmsg->isType(sgui::DrawMessage::Type::SHUTDOWN)) {
                    LOG_WARN("Shutdown - received shutdown draw commands");
                    shutdownInProgress = true;
                }

                if (dmsg->isType(sgui::DrawMessage::Type::SUSPEND)) {
                    LOG_WARN("Suspended - received suspend draw commands");
                    suspendInProgress = true;
                }

                mode = data.getMode();

                if (fs.emplaceDrawData(std::move(data))) {
                    LOG_ERROR("gui fs : failed to push commands to process");
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

    sys::Message_t ServiceGUI::handleGUIRenderingFinished(sys::Message *message)
    {
        if (einkReady) {
            sendBuffer();
        }
        else if (!requestSent) {
            requestSent = true;
            sys::Bus::SendUnicast(std::make_shared<service::eink::StateRequest>(), service::name::eink, this);
        }
        return nullptr;
    }

    sys::Message_t ServiceGUI::handleGUIDisplayReady(sys::Message *message)
    {
        auto msg    = static_cast<GUIDisplayReady *>(message);
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

        /// TODO do it based on fs data ready
        sendBuffer();

        // sendToRender();
        return nullptr;
    }

} /* namespace sgui */
