// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ContextPool.hpp"
#include "DrawCommandsQueue.hpp"
#include "RenderCache.hpp"
#include "messages/RenderingFinished.hpp"
#include "Common.hpp"

#include <system/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Timers/TimerHandle.hpp>
#include <service-eink/Common.hpp>

#include <cstdint>
#include <memory>
#include <string>

namespace gui
{
    class Context;
    class DrawCommand;
} // namespace gui

namespace service::gui
{
    class WorkerGUI;

    class ServiceGUI : public sys::Service
    {
        friend WorkerGUI;

      public:
        explicit ServiceGUI(::gui::Size displaySize,
                            const std::string &name = service::name::gui,
                            std::string parent      = {});
        ~ServiceGUI() noexcept override;

        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;
        void ProcessCloseReason(sys::CloseReason closeReason) override;
        auto ProcessCloseReasonHandler(sys::CloseReason closeReason) -> void override;
        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;

      private:
        bool isRendering{};
        bool isDisplaying{};
        bool isClosing{};

        static void initAssetManagers();
        void registerMessageHandlers();

        void prepareDisplayEarly(::gui::RefreshModes refreshMode);
        void notifyRenderer(std::list<std::unique_ptr<::gui::DrawCommand>> &&commands, ::gui::RefreshModes refreshMode);
        void notifyRenderColorSchemeChange(::gui::ColorScheme &&scheme);
        void enqueueDrawCommands(DrawCommandsQueue::QueueItem &&item);
        void sendOnDisplay(::gui::Context *context, int contextId, ::gui::RefreshModes refreshMode);
        void scheduleContextRelease(int contextId);
        bool isNextFrameReady() const noexcept;
        bool isAnyFrameBeingRenderedOrDisplayed() const noexcept;
        void trySendNextFrame();

        sys::MessagePointer handleDrawMessage(sys::Message *message);
        sys::MessagePointer handleGUIRenderingFinished(sys::Message *message);
        sys::MessagePointer handleImageDisplayedNotification(sys::Message *message);
        sys::MessagePointer handleChangeColorScheme(sys::Message *message);

        ::gui::Size displaySize;
        std::unique_ptr<ContextPool> contextPool;
        std::unique_ptr<WorkerGUI> worker;
        std::unique_ptr<DrawCommandsQueue> commandsQueue;
        std::unique_ptr<::gui::ColorScheme> colorSchemeUpdate;
        RenderCache cache;
        sys::TimerHandle contextReleaseTimer;
    };
} // namespace service::gui

namespace sys
{
    template <>
    struct ManifestTraits<service::gui::ServiceGUI>
    {
        static auto GetManifest() -> ServiceManifest
        {
            ServiceManifest manifest;
            manifest.name         = service::name::gui;
            manifest.dependencies = {service::name::eink};
            return manifest;
        }
    };
} // namespace sys
