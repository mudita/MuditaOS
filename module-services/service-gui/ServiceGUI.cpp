// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceGUI.hpp"
#include "WorkerGUI.hpp"

#include "messages/DrawMessage.hpp"
#include "messages/ChangeColorScheme.hpp"

#include <DrawCommand.hpp>
#include <FontManager.hpp>
#include <gui/core/ImageManager.hpp>
#include <log/log.hpp>
#include <service-eink/Common.hpp>
#include <service-eink/messages/ImageMessage.hpp>
#include <service-eink/messages/EinkMessage.hpp>
#include <service-eink/messages/PrepareDisplayEarlyRequest.hpp>
#include <Timers/TimerFactory.hpp>
#include <SystemManager/SystemManagerCommon.hpp>

#include <gsl/util>
#include <purefs/filesystem_paths.hpp>

namespace service::gui
{
    namespace
    {
        constexpr auto ServiceGuiStackDepth  = 1536U;
        constexpr auto ContextsCount         = 2;
        constexpr auto CommandsQueueCapacity = 3;
        constexpr std::chrono::milliseconds BSPEinkBusyTimeout{3000}; ///< sync with \ref BSP_EinkBusyTimeout
        constexpr std::chrono::milliseconds RTOSMessageRoundtripTimeout{1000};
        constexpr std::chrono::milliseconds ContextReleaseTimeout{BSPEinkBusyTimeout + RTOSMessageRoundtripTimeout};

        ::gui::RefreshModes getMaxRefreshMode(::gui::RefreshModes lhs, ::gui::RefreshModes rhs) noexcept
        {
            if (lhs == ::gui::RefreshModes::GUI_REFRESH_DEEP) {
                return lhs;
            }
            return rhs;
        }
    } // namespace

    ServiceGUI::ServiceGUI(::gui::Size displaySize, const std::string &name, std::string parent)
        : sys::Service(name, parent, ServiceGuiStackDepth), displaySize{displaySize},
          commandsQueue{std::make_unique<DrawCommandsQueue>(CommandsQueueCapacity)}
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
        contextPool = std::make_unique<ContextPool>(displaySize, ContextsCount);

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

    void ServiceGUI::ProcessCloseReason(sys::CloseReason closeReason)
    {
        if (not isDisplaying and not isRendering) {
            sendCloseReadyMessage(this);
            return;
        }
        else {
            isClosing = true;
        }
    }

    void ServiceGUI::ProcessCloseReasonHandler(sys::CloseReason closeReason)
    {
        ProcessCloseReason(closeReason);
    }

    sys::ReturnCodes ServiceGUI::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
    {
        LOG_INFO("PowerModeHandler: %s", c_str(mode));
        return sys::ReturnCodes::Success;
    }

    sys::MessagePointer ServiceGUI::handleDrawMessage(sys::Message *message)
    {
        if (const auto drawMsg = static_cast<DrawMessage *>(message); !drawMsg->commands.empty()) {
            if (!isAnyFrameBeingRenderedOrDisplayed()) {
                prepareDisplayEarly(drawMsg->mode);
            }
            notifyRenderer(std::move(drawMsg->commands), drawMsg->mode);
        }
        return std::make_shared<sys::ResponseMessage>();
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
        isRendering = true;
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
        isRendering          = false;
        auto finishedMsg     = static_cast<service::gui::RenderingFinished *>(message);
        const auto contextId = finishedMsg->getContextId();
        auto refreshMode     = finishedMsg->getRefreshMode();

        if (not isDisplaying) {
            if (cache.isRenderCached()) {
                refreshMode = getMaxRefreshMode(cache.getCachedRender()->refreshMode, refreshMode);
                cache.invalidate();
            }
            const auto context = contextPool->peekContext(contextId);
            sendOnDisplay(context, contextId, refreshMode);
        }
        else {
            cache.cache({contextId, refreshMode});
            contextPool->returnContext(contextId);
        }
        return sys::MessageNone{};
    }

    void ServiceGUI::sendOnDisplay(::gui::Context *context, int contextId, ::gui::RefreshModes refreshMode)
    {
        isDisplaying = true;
        auto msg     = std::make_shared<service::eink::ImageMessage>(contextId, context, refreshMode);
        bus.sendUnicast(std::move(msg), service::name::eink);
        scheduleContextRelease(contextId);
    }

    void ServiceGUI::scheduleContextRelease(int contextId)
    {
        // Whenever the response from ServiceEink doesn't come, the context has to be released automatically after a
        // timeout.
        contextReleaseTimer = sys::TimerFactory::createSingleShotTimer(
            this, "contextRelease", ContextReleaseTimeout, [this, contextId](sys::Timer &it) {
                eink::ImageDisplayedNotification notification{contextId};
                handleImageDisplayedNotification(&notification);
                LOG_WARN("Context #%d released after timeout. Does ServiceEink respond properly?", contextId);
            });
        contextReleaseTimer.start();
    }

    sys::MessagePointer ServiceGUI::handleImageDisplayedNotification(sys::Message *message)
    {
        isDisplaying         = false;
        const auto msg       = static_cast<eink::ImageDisplayedNotification *>(message);
        const auto contextId = msg->getContextId();
        contextPool->returnContext(contextId);
        contextReleaseTimer.stop();

        if (isClosing) {
            sendCloseReadyMessage(this);
        }
        else {
            // Even if the next render is already cached, if any context in the pool is currently being processed, then
            // we better wait for it.
            if (isNextFrameReady() and not isAnyFrameBeingRenderedOrDisplayed()) {
                trySendNextFrame();
            }
        }
        return sys::MessageNone{};
    }

    bool ServiceGUI::isNextFrameReady() const noexcept
    {
        return cache.isRenderCached();
    }

    bool ServiceGUI::isAnyFrameBeingRenderedOrDisplayed() const noexcept
    {
        return contextPool->isAnyContextLocked();
    }

    void ServiceGUI::trySendNextFrame()
    {
        const auto contextId = cache.getCachedRender()->contextId;
        if (const auto context = contextPool->borrowContext(contextId); context != nullptr) {
            sendOnDisplay(context, contextId, cache.getCachedRender()->refreshMode);
        }
        cache.invalidate();
    }
} // namespace service::gui
