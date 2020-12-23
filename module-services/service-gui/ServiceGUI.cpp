// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceGUI.hpp"
#include "WorkerGUI.hpp"

#include "messages/DrawMessage.hpp"
#include "messages/RenderingFinished.hpp"
#include "messages/EinkReady.hpp"

#include <DrawCommand.hpp>
#include <FontManager.hpp>
#include <gui/core/ImageManager.hpp>
#include <log/log.hpp>
#include <service-eink/Common.hpp>
#include <service-eink/messages/ImageMessage.hpp>
#include <service-eink/messages/EinkMessage.hpp>
#include <SystemManager/SystemManager.hpp>

#include <gsl/gsl_util>
#include <purefs/filesystem_paths.hpp>

namespace service::gui
{
    namespace
    {
        constexpr auto ServiceGuiStackDepth   = 4096U;
        constexpr auto RenderCommandSize      = sizeof(RenderCommand);
        constexpr auto ContextsCount          = 2;
        constexpr auto RenderingQueueCapacity = 10;
    } // namespace

    ServiceGUI::ServiceGUI(const std::string &name, std::string parent)
        : sys::Service(name, parent, ServiceGuiStackDepth), currentState{State::NotInitialised}
    {
        LOG_INFO("Initializing");
        initAssetManagers();
        registerMessageHandlers();
    }

    ServiceGUI::~ServiceGUI() noexcept = default;

    void ServiceGUI::initAssetManagers()
    {
        const auto assetsPath = purefs::dir::getCurrentOSPath() / "assets";
        ::gui::FontManager::getInstance().init(assetsPath);
        ::gui::ImageManager::getInstance().init(assetsPath);
    }

    void ServiceGUI::registerMessageHandlers()
    {
        connect(typeid(EinkReady),
                [this](sys::Message *request) -> sys::MessagePointer { return handleEinkReady(request); });

        connect(typeid(DrawMessage),
                [this](sys::Message *request) -> sys::MessagePointer { return handleDrawMessage(request); });

        connect(typeid(RenderingFinished),
                [this](sys::Message *request) -> sys::MessagePointer { return handleGUIRenderingFinished(request); });
    }

    sys::MessagePointer ServiceGUI::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        return sys::MessageNone{};
    }

    sys::ReturnCodes ServiceGUI::InitHandler()
    {
        std::list<sys::WorkerQueueInfo> queueInfo{
            {WorkerGUI::RenderingQueueName, RenderCommandSize, RenderingQueueCapacity}};
        worker = std::make_unique<WorkerGUI>(this);
        worker->init(queueInfo);
        worker->run();
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
        LOG_INFO("PowerModeHandler: %s", c_str(mode));
        switch (mode) {
        case sys::ServicePowerMode::Active:
            setState(contextPool != nullptr ? State::Running : State::NotInitialised);
            break;
        case sys::ServicePowerMode::SuspendToRAM:
            [[fallthrough]];
        case sys::ServicePowerMode::SuspendToNVM:
            setState(State::Suspended);
            break;
        }
        return sys::ReturnCodes::Success;
    }

    sys::MessagePointer ServiceGUI::handleDrawMessage(sys::Message *message)
    {
        if (isInState(State::NotInitialised)) {
            LOG_WARN("Service not yet initialised - ignoring draw commands");
            return sys::MessageNone{};
        }
        if (isInState(State::Suspended)) {
            LOG_WARN("Suspended - ignoring draw commands");
            return sys::MessageNone{};
        }

        if (const auto drawMsg = static_cast<DrawMessage *>(message); !drawMsg->commands.empty()) {

            if (drawMsg->isType(DrawMessage::Type::SHUTDOWN) || drawMsg->isType(DrawMessage::Type::SUSPEND)) {
                setState(State::Suspended);
            }
            notifyRenderer(std::move(drawMsg->commands), drawMsg->mode);
        }
        return sys::MessageNone{};
    }

    void ServiceGUI::notifyRenderer(std::list<std::unique_ptr<::gui::DrawCommand>> &&commands,
                                    ::gui::RefreshModes refreshMode)
    {
        auto renderCommand        = RenderCommand::fromCommandList(std::move(commands));
        renderCommand.refreshMode = refreshMode;
        worker->render(std::move(renderCommand));
    }

    sys::MessagePointer ServiceGUI::handleGUIRenderingFinished(sys::Message *message)
    {
        auto finishedMsg         = static_cast<service::gui::RenderingFinished *>(message);
        const auto contextId     = finishedMsg->getContextId();
        const auto isDeepRefresh = finishedMsg->isDeepRefreshRequested();

        const auto context = contextPool->peakContext(contextId);
        auto freeContext   = gsl::finally([this, contextId]() { contextPool->returnContext(contextId); });

        // Asynchronous send doesn't guarantee that any response will ever be received by the caller.
        // Receiving the response is essential, because the context has to be returned to the pool.
        // This is the reason for sending the message with blocking unicast.
        auto msg = std::make_shared<service::eink::ImageMessage>(
            context->getData(), context->getW(), context->getH(), isDeepRefresh, contextId);
        sys::Bus::SendUnicast(msg, service::name::eink, this, sys::defaultCmdTimeout);
        return sys::MessageNone{};
    }

    sys::MessagePointer ServiceGUI::handleEinkReady(sys::Message *message)
    {
        const auto msg = static_cast<service::gui::EinkReady *>(message);
        contextPool    = std::make_unique<ContextPool>(msg->getDisplaySize(), ContextsCount);
        setState(State::Running);
        return sys::MessageNone{};
    }

    void ServiceGUI::setState(State state) noexcept
    {
        currentState = state;
    }

    bool ServiceGUI::isInState(State state) const noexcept
    {
        return currentState == state;
    }
} /* namespace sgui */
