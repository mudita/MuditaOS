// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <board.h>
#include "ServiceEink.hpp"
#include "internal/StaticData.hpp"
#include "messages/EinkModeMessage.hpp"
#include "messages/PrepareDisplayEarlyRequest.hpp"
#include <time/ScopedTime.hpp>
#include <Timers/TimerFactory.hpp>

#include <log/log.hpp>
#include <messages/EinkMessage.hpp>
#include <messages/ImageMessage.hpp>
#include <system/messages/DeviceRegistrationMessage.hpp>
#include <system/messages/SentinelRegistrationMessage.hpp>
#include <system/Constants.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>

#include <cstring>
#include <memory>
#include <gsl/util>
#include "Utils.hpp"

namespace service::eink
{
    namespace
    {
        constexpr auto ServceEinkStackDepth = 4096U;
        constexpr std::chrono::milliseconds displayPowerOffTimeout{2000};

        std::string toSettingString(const EinkModeMessage::Mode mode)
        {
            if (mode == EinkModeMessage::Mode::Normal) {
                return "0";
            }
            return "1";
        }

        hal::eink::EinkRefreshMode translateToEinkRefreshMode(const gui::RefreshModes guiRefreshMode)
        {
            switch (guiRefreshMode) {
            case gui::RefreshModes::GUI_REFRESH_DEEP:
                return hal::eink::EinkRefreshMode::REFRESH_DEEP;
            case gui::RefreshModes::GUI_REFRESH_FAST:
                return hal::eink::EinkRefreshMode::REFRESH_FAST;
            default:
                return hal::eink::EinkRefreshMode::REFRESH_NONE;
            }
        }
    } // namespace

    ServiceEink::ServiceEink(ExitAction exitAction, const std::string &name, std::string parent)
        : sys::Service(name, std::move(parent), ServceEinkStackDepth), exitAction{exitAction},
          currentState{State::Running}, display{hal::eink::AbstractEinkDisplay::Factory::create(
                                            hal::eink::FrameSize{BOARD_EINK_DISPLAY_RES_X, BOARD_EINK_DISPLAY_RES_Y})},
          settings{std::make_unique<settings::Settings>()}
    {
        displayPowerOffTimer = sys::TimerFactory::createSingleShotTimer(
            this, "einkDisplayPowerOff", displayPowerOffTimeout, [this](sys::Timer &) {
                display->powerOff();
                eInkSentinel->ReleaseMinimumFrequency();
            });
        connect(typeid(EinkModeMessage),
                [this](sys::Message *message) -> sys::MessagePointer { return handleEinkModeChangedMessage(message); });

        connect(typeid(ImageMessage),
                [this](sys::Message *request) -> sys::MessagePointer { return handleImageMessage(request); });

        connect(typeid(PrepareDisplayEarlyRequest),
                [this](sys::Message *request) -> sys::MessagePointer { return handlePrepareEarlyRequest(request); });

        eInkSentinel = std::make_shared<EinkSentinel>(name::eink, this);
    }

    sys::MessagePointer ServiceEink::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *response)
    {
        return std::make_shared<sys::ResponseMessage>();
    }

    sys::ReturnCodes ServiceEink::InitHandler()
    {
        LOG_INFO("Initializing");
        if (const auto status = display->resetAndInit(); status != hal::eink::EinkStatus::EinkOK) {
            LOG_FATAL("Error: Could not initialize Eink display!");
            return sys::ReturnCodes::Failure;
        }

        settings->init(service::ServiceProxy(shared_from_this()));
        initStaticData();

        auto deviceRegistrationMsg = std::make_shared<sys::DeviceRegistrationMessage>(display->getDevice());
        bus.sendUnicast(deviceRegistrationMsg, service::name::system_manager);

        auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(eInkSentinel);
        bus.sendUnicast(sentinelRegistrationMsg, service::name::system_manager);

        display->powerOn();
        eInkSentinel->HoldMinimumFrequency();

        return sys::ReturnCodes::Success;
    }

    void ServiceEink::initStaticData()
    {
        const auto invertedModeSetting   = settings->getValue(settings::Display::invertedMode);
        const auto isInvertedModeEnabled = utils::toNumeric(invertedModeSetting);
        const auto mode = isInvertedModeEnabled == 0 ? EinkModeMessage::Mode::Normal : EinkModeMessage::Mode::Invert;
        setDisplayMode(mode);
    }

    sys::ReturnCodes ServiceEink::DeinitHandler()
    {
        // Eink must be turn on before wiping out the display
        display->powerOn();
        if (exitAction == ExitAction::WipeOut) {
            display->wipeOut();
        }
        display->shutdown();
        settings->deinit();
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ServiceEink::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
    {
        LOG_INFO("PowerModeHandler: %s", c_str(mode));
        switch (mode) {
        case sys::ServicePowerMode::Active:
            enterActiveMode();
            break;
        case sys::ServicePowerMode::SuspendToRAM:
            [[fallthrough]];
        case sys::ServicePowerMode::SuspendToNVM:
            suspend();
            break;
        }
        return sys::ReturnCodes::Success;
    }

    void ServiceEink::enterActiveMode()
    {
        setState(State::Running);

        if (const auto status = display->resetAndInit(); status != hal::eink::EinkStatus::EinkOK) {
            LOG_FATAL("Error: Could not initialize Eink display!");
        }
        eInkSentinel->HoldMinimumFrequency();
        display->powerOn();
        display->powerOff();
        eInkSentinel->ReleaseMinimumFrequency();
    }

    void ServiceEink::suspend()
    {
        setState(State::Suspended);
        display->shutdown();
    }

    sys::MessagePointer ServiceEink::handleEinkModeChangedMessage(sys::Message *message)
    {
        const auto msg  = static_cast<EinkModeMessage *>(message);
        const auto mode = msg->getMode();
        setDisplayMode(mode);
        settings->setValue(settings::Display::invertedMode, toSettingString(mode));
        return std::make_shared<EinkModeResponse>();
    }

    void ServiceEink::setDisplayMode(EinkModeMessage::Mode mode)
    {
        auto invertedModeRequested = mode == EinkModeMessage::Mode::Invert;
        if (invertedModeRequested) {
            display->setMode(hal::eink::EinkDisplayColorMode::EinkDisplayColorModeInverted);
        }
        else {
            display->setMode(hal::eink::EinkDisplayColorMode::EinkDisplayColorModeStandard);
        }
        internal::StaticData::get().setInvertedMode(invertedModeRequested);

        previousContext.reset();
        previousRefreshMode = hal::eink::EinkRefreshMode::REFRESH_NONE;
    }

    // Merge boxes if the gap between them is lesser than the threshold
    template <typename BoxesContainer>
    inline std::vector<gui::BoundingBox> mergeBoundingBoxes(const BoxesContainer &boxes, std::uint16_t gapThreshold)
    {
        std::vector<gui::BoundingBox> mergedBoxes;
        if (boxes.empty())
            return mergedBoxes;
        mergedBoxes.reserve(boxes.size());
        gui::BoundingBox merged = boxes.front();
        for (std::size_t i = 1; i < boxes.size(); ++i) {
            const auto &bb = boxes[i];
            const auto gap = bb.y - (merged.y + merged.h);
            if (gap < gapThreshold) {
                merged.h = (bb.y + bb.h) - merged.y;
            }
            else {
                mergedBoxes.push_back(merged);
                merged = bb;
            }
        }
        mergedBoxes.push_back(merged);
        return mergedBoxes;
    }

    // Enlarge each box to match alignment-wide grid in y coordinate
    template <typename BoxesContainer>
    inline std::vector<hal::eink::EinkFrame> makeAlignedFrames(const BoxesContainer &boxes, std::uint16_t alignment)
    {
        std::vector<hal::eink::EinkFrame> frames;
        if (boxes.empty())
            return frames;
        frames.reserve(boxes.size());
        auto a = alignment;
        for (const auto &bb : boxes) {
            auto f = hal::eink::EinkFrame{
                std::uint16_t(bb.x), std::uint16_t(bb.y), {std::uint16_t(bb.w), std::uint16_t(bb.h)}};
            auto y        = f.pos_y;
            auto h        = f.size.height;
            f.pos_y       = y / a * a;
            f.size.height = (y - f.pos_y + h + (a - 1)) / a * a;
            frames.push_back(f);
        }
        return frames;
    }

#if DEBUG_EINK_REFRESH == 1
    inline std::string debug_toString(const gui::BoundingBox &bb)
    {
        return bb.str();
    }

    inline std::string debug_toString(const hal::eink::EinkFrame &f)
    {
        std::stringstream ss;
        ss << '{' << f.pos_x << ' ' << f.pos_y << ' ' << f.size.width << ' ' << f.size.height << "}";
        return ss.str();
    }

    template <typename Container>
    inline void debug_handleImageMessage(const char *name, const Container &container)
    {
        std::stringstream ss;
        for (auto const &el : container)
            ss << debug_toString(el) << ' ';
        LOG_INFO("%s: %s", name, ss.str().c_str());
    }
#endif

    sys::MessagePointer ServiceEink::handleImageMessage(sys::Message *request)
    {
        const auto message = static_cast<service::eink::ImageMessage *>(request);
        if (isInState(State::Suspended)) {
            LOG_WARN("Received image while suspended, ignoring");
            return sys::MessageNone{};
        }
        utils::time::Scoped measurement("ImageMessage");

        displayPowerOffTimer.stop();
        auto displayPowerOffTimerReload = gsl::finally([this]() { displayPowerOffTimer.start(); });

        const gui::Context &ctx = *message->getContext();
        auto refreshMode        = translateToEinkRefreshMode(message->getRefreshMode());

        // Get areas changed since last update
        std::vector<hal::eink::EinkFrame> updateFrames;
        if (!previousContext) {
            updateFrames = {{0, 0, {ctx.getW(), ctx.getH()}}};

            previousContext.reset(new gui::Context(ctx.get(0, 0, ctx.getW(), ctx.getH())));
        }
        else if (refreshMode > previousRefreshMode) {
            updateFrames = {{0, 0, {ctx.getW(), ctx.getH()}}};

            previousContext->insert(0, 0, ctx);
        }
        else {
            // Each bounding box cover the whole width of the context. They are disjoint and sorted by y coordinate.
            const auto diffBoundingBoxes = gui::Context::linesDiffs(ctx, *previousContext);
            if (!diffBoundingBoxes.empty()) {
                const std::uint16_t gapThreshold = ctx.getH() / 4;
                const std::uint16_t alignment    = 8;

                const auto mergedBoxes = mergeBoundingBoxes(diffBoundingBoxes, gapThreshold);
                updateFrames           = makeAlignedFrames(mergedBoxes, alignment);

#if DEBUG_EINK_REFRESH == 1
                debug_handleImageMessage("Boxes", diffBoundingBoxes);
                debug_handleImageMessage("Merged boxes", mergedBoxes);
#endif

                previousContext->insert(0, 0, ctx);
            }
        }

        if (!updateFrames.empty()) {
            hal::eink::EinkFrame refreshFrame;
            refreshFrame = updateFrames.front();
            refreshFrame.size.height =
                updateFrames.back().pos_y + updateFrames.back().size.height - updateFrames.front().pos_y;

            eInkSentinel->HoldMinimumFrequency();

            const auto status = display->showImage(updateFrames, refreshFrame, ctx.getData(), refreshMode);
            if (status != hal::eink::EinkStatus::EinkOK) {
                previousContext.reset();
                refreshMode = hal::eink::EinkRefreshMode::REFRESH_NONE;
                LOG_ERROR("Error during drawing image on eink: %s", magic_enum::enum_name(status).data());
            }

#if DEBUG_EINK_REFRESH == 1
            debug_handleImageMessage("Frames", updateFrames);
            debug_handleImageMessage("Refresh frame", std::vector<hal::eink::EinkFrame>({refreshFrame}));
#endif
        }

#if DEBUG_EINK_REFRESH == 1
        LOG_INFO("Refresh context: %d %d, mode: %d", (int)ctx.getW(), (int)ctx.getH(), (int)refreshMode);
#endif

        previousRefreshMode = refreshMode;

        return std::make_shared<service::eink::ImageDisplayedNotification>(message->getContextId());
    }

    sys::MessagePointer ServiceEink::handlePrepareEarlyRequest(sys::Message *message)
    {
        const auto waveformUpdateMsg = static_cast<service::eink::PrepareDisplayEarlyRequest *>(message);
        display->prepareEarlyRequest(translateToEinkRefreshMode(waveformUpdateMsg->getRefreshMode()),
                                     hal::eink::WaveformTemperature::MEASURE_NEW);
        return sys::MessageNone{};
    }

    void ServiceEink::setState(State state) noexcept
    {
        currentState = state;
    }

    bool ServiceEink::isInState(State state) const noexcept
    {
        return currentState == state;
    }

} // namespace service::eink
