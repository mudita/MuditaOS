﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceGUI.hpp"
#include "WorkerGUI.hpp"

#include "messages/DrawMessage.hpp"

#include "messages/EinkInitialized.hpp"
#include "messages/ChangeColorScheme.hpp"

#include <DrawCommand.hpp>
#include <FontManager.hpp>
#include <gui/core/ImageManager.hpp>
#include <log/log.hpp>
#include <service-eink/Common.hpp>
#include <service-eink/messages/ImageMessage.hpp>
#include <service-eink/messages/EinkMessage.hpp>
#include <service-eink/messages/PrepareDisplayEarlyRequest.hpp>
#include <SystemManager/SystemManager.hpp>

#include <gsl/gsl_util>
#include <purefs/filesystem_paths.hpp>

namespace service::gui
{
    namespace
    {
        constexpr auto ServiceGuiStackDepth  = 4096U;
        constexpr auto ContextsCount         = 2;
        constexpr auto CommandsQueueCapacity = 3;
        constexpr std::chrono::milliseconds BSPEinkBusyTimeout{3000}; ///< sync with \ref BSP_EinkBusyTimeout
        constexpr std::chrono::milliseconds RTOSMessageRoundtripTimeout{1000};
        constexpr std::chrono::milliseconds ContextReleaseTimeout{BSPEinkBusyTimeout + RTOSMessageRoundtripTimeout};
    } // namespace

    ServiceGUI::ServiceGUI(const std::string &name, std::string parent)
        : sys::Service(name, parent, ServiceGuiStackDepth), commandsQueue{std::make_unique<DrawCommandsQueue>(
                                                                CommandsQueueCapacity)},
          contextReleaseTimer{
              std::make_unique<sys::Timer>(this, ContextReleaseTimeout.count(), sys::Timer::Type::SingleShot)},
          currentState{State::NotInitialised}
    {
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
        connect(typeid(EinkInitialized),
                [this](sys::Message *request) -> sys::MessagePointer { return handleEinkInitialized(request); });

        connect(typeid(DrawMessage),
                [this](sys::Message *request) -> sys::MessagePointer { return handleDrawMessage(request); });

        connect(typeid(RenderingFinished),
                [this](sys::Message *request) -> sys::MessagePointer { return handleGUIRenderingFinished(request); });

        connect(typeid(ChangeColorScheme),
                [this](sys::Message *request) -> sys::MessagePointer { return handleChangeColorScheme(request); });

        connect(typeid(eink::ImageDisplayedNotification), [this](sys::Message *request) -> sys::MessagePointer {
            return handleImageDisplayedNotification(request);
        });
    }

    sys::MessagePointer ServiceGUI::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        return sys::MessageNone{};
    }

    sys::ReturnCodes ServiceGUI::InitHandler()
    {
        std::list<sys::WorkerQueueInfo> queueInfo{
            {WorkerGUI::SignallingQueueName, WorkerGUI::SignalSize, WorkerGUI::SignallingQueueCapacity}};
        worker = std::make_unique<WorkerGUI>(this);
        worker->init(queueInfo);
        worker->run();
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ServiceGUI::DeinitHandler()
    {
        worker->close();
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ServiceGUI::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
    {
        LOG_INFO("PowerModeHandler: %s", c_str(mode));
        switch (mode) {
        case sys::ServicePowerMode::Active:
            setState(contextPool != nullptr ? State::Idle : State::NotInitialised);
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

            if (!contextPool->isAnyContextLocked()) {
                prepareDisplayEarly(drawMsg->mode);
            }
            notifyRenderer(std::move(drawMsg->commands), drawMsg->mode);
        }
        return sys::MessageNone{};
    }

    sys::MessagePointer ServiceGUI::handleChangeColorScheme(sys::Message *message)
    {
        const auto msg = static_cast<ChangeColorScheme *>(message);
        notifyRenderColorSchemeChange(msg->getColorScheme());
        return sys::MessageNone{};
    }

    void ServiceGUI::prepareDisplayEarly(::gui::RefreshModes refreshMode)
    {
        auto msg = std::make_shared<service::eink::PrepareDisplayEarlyRequest>(refreshMode);
        bus.sendUnicast(msg, service::name::eink);
    }

    void ServiceGUI::notifyRenderer(std::list<std::unique_ptr<::gui::DrawCommand>> &&commands,
                                    ::gui::RefreshModes refreshMode)
    {
        enqueueDrawCommands(DrawCommandsQueue::QueueItem{std::move(commands), refreshMode});
        worker->notify(WorkerGUI::Signal::Render);
    }

    void ServiceGUI::notifyRenderColorSchemeChange(::gui::ColorScheme &&scheme)
    {
        colorSchemeUpdate = std::make_unique<::gui::ColorScheme>(scheme);
        worker->notify(WorkerGUI::Signal::ChangeColorScheme);
    }

    void ServiceGUI::enqueueDrawCommands(DrawCommandsQueue::QueueItem &&item)
    {
        // Clear all queue elements for now to keep only the latest command in the queue.
        // In the future, we'll need to implement more sophisticated algorithm for partially refresh the display.
        if (item.refreshMode == ::gui::RefreshModes::GUI_REFRESH_DEEP) {
            commandsQueue->clear();
        }
        else if (const auto maxRefreshMode = commandsQueue->getMaxRefreshModeAndClear();
                 maxRefreshMode == ::gui::RefreshModes::GUI_REFRESH_DEEP) {
            item.refreshMode = maxRefreshMode;
        }
        commandsQueue->enqueue(std::move(item)); // 3 consecutive deep refreshes after leaving messages? :/
    }

    sys::MessagePointer ServiceGUI::handleGUIRenderingFinished(sys::Message *message)
    {
        auto finishedMsg       = static_cast<service::gui::RenderingFinished *>(message);
        const auto contextId   = finishedMsg->getContextId();
        const auto refreshMode = finishedMsg->getRefreshMode();
        if (isInState(State::Idle)) {
            const auto context = contextPool->peekContext(contextId);
            sendOnDisplay(context, contextId, refreshMode);
            invalidateCache();
        }
        else {
            cacheRender(contextId, refreshMode);
            contextPool->returnContext(contextId);
        }
        return sys::MessageNone{};
    }

    void ServiceGUI::sendOnDisplay(::gui::Context *context, int contextId, ::gui::RefreshModes refreshMode)
    {
        setState(State::Busy);
        auto imageMsg = std::make_shared<service::eink::ImageMessage>(contextId, context, refreshMode);
        bus.sendUnicast(imageMsg, service::name::eink);
        scheduleContextRelease(contextId);
    }

    void ServiceGUI::scheduleContextRelease(int contextId)
    {
        // Whenever the response from ServiceEink doesn't come, the context has to be released automatically after a
        // timeout.
        contextReleaseTimer->connect([this, contextId](sys::Timer &it) {
            eink::ImageDisplayedNotification notification{contextId};
            handleImageDisplayedNotification(&notification);
            LOG_WARN("Context # %d released after timeout. Does ServiceEink respond properly?", contextId);
        });
        contextReleaseTimer->start();
    }

    void ServiceGUI::cacheRender(int contextId, ::gui::RefreshModes refreshMode)
    {
        cachedRender = CachedRender{contextId, refreshMode};
    }

    void ServiceGUI::invalidateCache()
    {
        cachedRender = std::nullopt;
    }

    sys::MessagePointer ServiceGUI::handleEinkInitialized(sys::Message *message)
    {
        const auto msg = static_cast<service::gui::EinkInitialized *>(message);
        contextPool    = std::make_unique<ContextPool>(msg->getDisplaySize(), ContextsCount);
        setState(State::Idle);
        return sys::MessageNone{};
    }

    sys::MessagePointer ServiceGUI::handleImageDisplayedNotification(sys::Message *message)
    {
        const auto msg       = static_cast<eink::ImageDisplayedNotification *>(message);
        const auto contextId = msg->getContextId();
        contextPool->returnContext(contextId);
        contextReleaseTimer->stop();
        setState(State::Idle);

        if (isNextFrameReady()) {
            trySendNextFrame();
        }
        return sys::MessageNone{};
    }

    bool ServiceGUI::isNextFrameReady() const noexcept
    {
        // Even if the next render is already cached, if any context in the pool is currently being processed, then we
        // better wait for it.
        return cachedRender.has_value() && !contextPool->isAnyContextLocked();
    }

    void ServiceGUI::trySendNextFrame()
    {
        const auto contextId = cachedRender->contextId;
        if (const auto context = contextPool->borrowContext(contextId); context != nullptr) {
            sendOnDisplay(context, contextId, cachedRender->refreshMode);
        }
        invalidateCache();
    }

    void ServiceGUI::setState(State state) noexcept
    {
        currentState = state;
    }

    bool ServiceGUI::isInState(State state) const noexcept
    {
        return currentState == state;
    }
} // namespace service::gui
