/*
 * ServiceGUI.cpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

#include <memory>

// module-os
#include "FreeRTOS.h"
#include "semphr.h"

// module-gui
#include "gui/core/Font.hpp"
#include "gui/core/Context.hpp"
#include "gui/input/Translator.hpp"

// gui service
#include "messages/GUIMessage.hpp"
#include "messages/DrawMessage.hpp"

// service-eink
#include "service-eink/messages/ImageMessage.hpp"

#include "ServiceGUI.hpp"
#include "service-appmgr/ApplicationManager.hpp"

#include "../gui/core/ImageManager.hpp"
#include "log/log.hpp"

extern "C"
#include "memory/usermem.h"

#include "SystemManager/SystemManager.hpp"
#include "WorkerGUI.hpp"

    namespace sgui
{

    static uint32_t getTimeFunction()
    {
        return xTaskGetTickCount();
    }

    ServiceGUI::ServiceGUI(const std::string &name, std::string parent, uint32_t screenWidth, uint32_t screenHeight)
        : sys::Service(name, parent, 4096, sys::ServicePriority::Idle), renderContext{nullptr},
          transferContext{nullptr}, renderFrameCounter{1}, transferedFrameCounter{0}, screenWidth{screenWidth},
          screenHeight{screenHeight}, semCommands{NULL}, worker{nullptr}
    {

        LOG_INFO("[ServiceGUI] Initializing");

        // allocate buffers for rendering and transferring data to eink
        renderContext   = new gui::Context(screenWidth, screenHeight);
        transferContext = new gui::Context(screenWidth, screenHeight);

        // load fonts
        gui::FontManager &fontManager = gui::FontManager::getInstance();
        fontManager.init("sys/assets");

        // load images
        gui::ImageManager &imageManager = gui::ImageManager::getInstance();
        imageManager.init("sys/assets");
    }

    ServiceGUI::~ServiceGUI()
    {
        LOG_INFO("[ServiceGUI] Cleaning resources");
        if (renderContext)
            delete renderContext;
        if (transferContext)
            delete transferContext;

        worker->deinit();

        gui::ImageManager::getInstance().clear();
        gui::FontManager::getInstance().clear();
    }

    void ServiceGUI::sendBuffer()
    {
        // copy data from render context to transfer context
        transferContext->insert(0, 0, renderContext);

        auto msg  = std::make_shared<seink::ImageMessage>(0,
                                                         0,
                                                         transferContext->getW(),
                                                         transferContext->getH(),
                                                         (mode == gui::RefreshModes::GUI_REFRESH_DEEP ? true : false),
                                                         transferContext->getData(),
                                                         suspendInProgress,
                                                         shutdownInProgress);
        einkReady = false;
        auto ret  = sys::Bus::SendUnicast(msg, "ServiceEink", this, 2000);
        if (ret.first == sys::ReturnCodes::Success) {
            transferedFrameCounter = renderFrameCounter;
        }
        // set default refreshing mode.
        mode = gui::RefreshModes::GUI_REFRESH_FAST;
    }

    void ServiceGUI::sendToRender()
    {
        rendering = true;
        worker->send(static_cast<uint32_t>(WorkerGUICommands::Render), NULL);
    }

    // Invoked upon receiving data message
    sys::Message_t ServiceGUI::DataReceivedHandler(sys::DataMessage * msgl, sys::ResponseMessage * resp)
    {

        sgui::GUIMessage *msg = static_cast<sgui::GUIMessage *>(msgl);

        switch (msg->messageType) {
        case MessageType::MessageTypeUninitialized: {
            //			LOG_ERROR("[ServiceGUI] Received uninitialized message type");
        } break;
        case MessageType::GUICommands: {
            //			LOG_INFO("[%s] GUICommands", GetName().c_str());
            auto dmsg = static_cast<sgui::DrawMessage *>(msgl);
            if (!dmsg->commands.empty()) {

                // if suspend flag is set ignore any new message
                if (!suspendInProgress) {

                    if (dmsg->command == sgui::DrawMessage::DrawCommand::SHUTDOWN) {
                        LOG_WARN("Shutdown - received shutdown draw commands");
                        shutdownInProgress = true;
                    }

                    // if message carries suspend flag set flag in service and proceed
                    if (dmsg->command == sgui::DrawMessage::DrawCommand::SUSPEND) {
                        LOG_WARN("Suspended - received suspend draw commands");
                        suspendInProgress = true;
                    }

                    // update mode
                    if (dmsg->mode == gui::RefreshModes::GUI_REFRESH_DEEP) {
                        mode = dmsg->mode;
                    }

                    //				LOG_INFO("[ServiceGUI] Received %d draw commands", dmsg->commands.size());

                    // lock access to commands vector, clear it and then copy commands from message to vector
                    if (xSemaphoreTake(semCommands, pdMS_TO_TICKS(1000)) == pdTRUE) {
                        commands.clear();
                        for (auto it = dmsg->commands.begin(); it != dmsg->commands.end(); it++)
                            commands.push_back(std::move(*it));
                        xSemaphoreGive(semCommands);
                    }
                    else {
                        LOG_ERROR("Failed to acquire semaphore");
                    }

                    // if worker is not rendering send him new set of commands
                    if (!rendering) {
                        sendToRender();
                    }
                    else {
                        //						LOG_ERROR("Already rendering");
                    }

                    //				uint32_t mem = usermemGetFreeHeapSize();
                    //				LOG_WARN( "Heap Memory: %d", mem );
                }
                else {
                    LOG_WARN("Suspended - ignoring draw commands");
                }
            }
        } break;
        case MessageType::GUIRenderingFinished: {
            //			LOG_INFO("[%s] GUIRenderingFinished", GetName().c_str());
            // increment counter holding number of drawn frames
            rendering = false;
            renderFrameCounter++;
            // copy render buffer to transfer buffer using semaphore to protect data
            // gui service is locking semaphore, makes a copy and then sends message to eink

            if (einkReady) {
                sendBuffer();
            }
            else if (!requestSent) {
                requestSent = true;
                // request eink state
                auto msg = std::make_shared<seink::EinkMessage>(MessageType::EinkStateRequest);
                sys::Bus::SendUnicast(msg, "ServiceEink", this);
            }
        } break;
        case MessageType::GUIFocusInfo: {
            //			LOG_INFO("[%s] GUIFocusInfo", GetName().c_str());
        } break;
        case MessageType::GUIDisplayReady: {
            //			LOG_INFO("[%s] GUIDisplayReady", GetName().c_str());
            einkReady   = true;
            requestSent = false;

            if (msg->getShutdown()) {
                einkReady         = false;
                suspendInProgress = false;
                LOG_DEBUG("last rendering before shutdown finished.");

                sys::SystemManager::CloseSystem(this);
            }

            if (msg->getSuspend()) {
                einkReady         = false;
                suspendInProgress = false;
                LOG_DEBUG("last rendering before suspend is finished.");

                sapm::ApplicationManager::messageInitPowerSaveMode(this);
            }
            // mode = gui::RefreshModes::GUI_REFRESH_FAST;
            // check if something new was rendered. If so render counter has greater value than
            // transfer counter.
            if ((renderFrameCounter != transferedFrameCounter) && (!rendering)) {
                //				LOG_INFO("[ServiceGUI]Sending buffer");
                sendBuffer();
            }
            else {
                //				LOG_INFO(" NO new buffer to send");
            }

            // check if there are pending commands to render.
            if (commands.empty() == false) {
                //				LOG_INFO("Rendering pending %d commands", commands.size());
                sendToRender();
            }
        } break;
        default:
            break;
        };

        return std::make_shared<sys::ResponseMessage>();
    }

    // Invoked when timer ticked
    void ServiceGUI::TickHandler(uint32_t id)
    {}

    // Invoked during initialization
    sys::ReturnCodes ServiceGUI::InitHandler()
    {

        // set function for acquiring time in seconds from the system
        gui::setTimeFunction(getTimeFunction);

        // create semaphore to protect vector with commands waiting for rendering
        semCommands = xSemaphoreCreateBinary();
        if (semCommands == NULL) {
            LOG_FATAL("Failed to create commands semaphore.");
            return sys::ReturnCodes::Failure;
        }
        xSemaphoreGive(semCommands);

        // initialize gui worker
        worker = new WorkerGUI(this);
        std::list<sys::WorkerQueueInfo> list;
        worker->init(list);
        worker->run();

        if (einkReady == false) {
            requestSent = true;
            auto msg    = std::make_shared<seink::EinkMessage>(MessageType::EinkStateRequest);
            sys::Bus::SendUnicast(msg, "ServiceEink", this);
        }
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ServiceGUI::DeinitHandler()
    {

        if (semCommands != NULL)
            vSemaphoreDelete(semCommands);
        semCommands = NULL;

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

} /* namespace sgui */
