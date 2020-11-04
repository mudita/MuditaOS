// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceEink.hpp"
#include "messages/EinkModeMessage.hpp"
#include "messages/EinkDMATransfer.hpp"
#include "messages/StateRequest.hpp"
#include "messages/TemperatureUpdate.hpp"
#include <service-gui/Common.hpp>
#include <service-gui/messages/GUIDisplayReady.hpp>
#include <time/ScopedTime.hpp>

#include <log/log.hpp>
#include <messages/EinkMessage.hpp>
#include <messages/ImageMessage.hpp>
#include <MessageType.hpp>
#include <Service/Bus.hpp>
#include <service-gui/messages/GUIMessage.hpp>
#include <vfs.hpp>

#include <cstdio>
#include <cstring>
#include <memory>

enum class EinkWorkerCommands
{
    Initialize,
    Initialized,
    Destroy,
    CopyImage,
    CopyCompleteCallback,
    CopyComplete
};

ServiceEink::ServiceEink(const std::string &name, std::string parent)
    : sys::Service(name, parent, 4096 + 1024),
      einkRenderBuffer(std::make_unique<uint8_t[]>(screen.height * screen.width))
{
    memset(&waveformSettings, 0, sizeof(EinkWaveFormSettings_t));
    waveformSettings.mode        = EinkWaveformGC16;
    waveformSettings.temperature = -1000;

    connect(typeid(service::eink::EinkModeMessage),
            [this](sys::DataMessage *message, sys::ResponseMessage *) -> sys::Message_t {
                auto msg          = static_cast<service::eink::EinkModeMessage *>(message);
                this->displayMode = msg->getMode() == service::eink::EinkModeMessage::Mode::Normal
                                        ? EinkDisplayColorMode_e::EinkDisplayColorModeStandard
                                        : EinkDisplayColorMode_e::EinkDisplayColorModeInverted;
                return sys::Message_t();
            });

    connect(typeid(service::eink::EinkDMATransfer),
            [&](sys::DataMessage *request, sys::ResponseMessage *) -> sys::Message_t {
                return handleEinkDMATransfer(request);
            });

    connect(typeid(service::eink::ImageMessage),
            [&](sys::DataMessage *request, sys::ResponseMessage *) -> sys::Message_t {
                return handleImageMessage(request);
            });

    connect(typeid(service::eink::StateRequest),
            [&](sys::DataMessage *request, sys::ResponseMessage *) -> sys::Message_t {
                return handleStateRequest(request);
            });

    connect(typeid(service::eink::TemperatureUpdate),
            [&](sys::DataMessage *request, sys::ResponseMessage *) -> sys::Message_t {
                return handleTemperatureUpdate(request);
            });
}

ServiceEink::~ServiceEink()
{
    if (waveformSettings.LUTCData != nullptr)
        delete[] waveformSettings.LUTCData;
    if (waveformSettings.LUTDData != nullptr)
        delete[] waveformSettings.LUTDData;
    waveformSettings.temperature = -1000;
}

sys::Message_t ServiceEink::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    return std::make_shared<sys::ResponseMessage>();
}

sys::ReturnCodes ServiceEink::InitHandler()
{

    LOG_INFO("[ServiceEink] Initializing");

    EinkStatus_e einkStatus = EinkResetAndInitialize();

    if (einkStatus != EinkOK) {
        LOG_FATAL("Error: Could not initialize Eink display!\n");
    }

    EinkPowerOn();

    auto msg = std::make_shared<service::gui::GUIDisplayReady>(suspendInProgress, shutdownInProgress);
    sys::Bus::SendUnicast(msg, service::name::gui, this);

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceEink::DeinitHandler()
{
    EinkPowerDown();
    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceEink::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    LOG_FATAL("[ServiceEink] PowerModeHandler: %s", c_str(mode));

    switch (mode) {
    case sys::ServicePowerMode ::Active: {
        suspended               = false;
        EinkStatus_e einkStatus = EinkResetAndInitialize();

        if (einkStatus != EinkOK) {
            LOG_FATAL("Error: Could not initialize Eink display!\n");
        }

        EinkPowerOn();
        EinkPowerOff();
    } break;
    case sys::ServicePowerMode ::SuspendToRAM:
    case sys::ServicePowerMode ::SuspendToNVM:
        suspended = true;
        EinkPowerDown();
        break;
    }

    return sys::ReturnCodes::Success;
}

bool ServiceEink::changeWaveform(EinkWaveforms_e mode, const int32_t temperature)
{

    if ((temperature == waveformSettings.temperature) && (mode == waveformSettings.mode)) {
        return EinkOK;
    }

    const uint32_t LUTD_SIZE = 16385;
    const uint32_t LUTC_SIZE = 64;
    const uint32_t LUTR_SIZE = 256; ///< Needed due to \ref EINK_LUTS_FILE_PATH structure

    const uint32_t LUTS_TOTAL_SIZE = LUTD_SIZE + LUTC_SIZE + LUTR_SIZE;

    waveformSettings.temperature = temperature;
    waveformSettings.mode        = mode;

    unsigned int segment = 0;

    if (temperature < 38) {
        segment = temperature / 3;
    }
    else {
        if (temperature < 43) {
            segment = 12;
        }
        else {
            segment = 13;
        }
    }

    uint32_t offset = 0;

    switch (mode) {
    case EinkWaveformINIT:
        offset = LUTS_TOTAL_SIZE * segment;
        break;

    case EinkWaveformA2:
        offset = LUTS_TOTAL_SIZE * (14 + segment);
        break;

    case EinkWaveformDU2:
        offset = LUTS_TOTAL_SIZE * (28 + segment);
        break;

    case EinkWaveformGLD16:
        offset = LUTS_TOTAL_SIZE * (42 + segment);
        break;

    case EinkWaveformGC16:
    default:
        offset = LUTS_TOTAL_SIZE * (56 + segment);
        break;
    }

    auto file = vfs.fopen("Luts.bin", "rb");
    if (file == nullptr) {
        LOG_FATAL("Could not find the LUTS.bin file. Returning");
        return false;
    }

    if (waveformSettings.LUTDData != nullptr)
        delete[] waveformSettings.LUTDData;

    waveformSettings.LUTDSize = 0;
    waveformSettings.LUTDData = new uint8_t[LUTD_SIZE + 1];

    if (waveformSettings.LUTDData == nullptr) {
        LOG_ERROR("Could not allocate memory for the LUTD array");
        vfs.fclose(file);
        return false;
    }

    if (waveformSettings.LUTCData != nullptr)
        delete[] waveformSettings.LUTCData;

    waveformSettings.LUTCSize = LUTC_SIZE;
    waveformSettings.LUTCData = new uint8_t[LUTC_SIZE + 1];
    if (waveformSettings.LUTCData == nullptr) {
        LOG_ERROR("Could not allocate memory for the LUTC array");
        vfs.fclose(file);
        return false;
    }

    waveformSettings.LUTDData[0] = EinkLUTD;
    waveformSettings.LUTCData[0] = EinkLUTC;

    vfs.fseek(file, offset, SEEK_SET);
    vfs.fread(&waveformSettings.LUTDData[1], 1, LUTD_SIZE, file);

    uint8_t frameCount = waveformSettings.LUTDData[1] + 1; // 0x00 - 1 frame, ... , 0x0F - 16 frames
    waveformSettings.LUTDSize =
        frameCount * 64 + 1 +
        1; // (frameCount * 64) - size of actual LUT; (+1) - the byte containing frameCount; (+1) - EinkLUTD command

    offset += LUTD_SIZE;
    vfs.fseek(file, offset, SEEK_SET);
    vfs.fread(&waveformSettings.LUTCData[1], 1, LUTC_SIZE, file);

    vfs.fclose(file);

    EinkUpdateWaveform(&waveformSettings);

    return true;
}

bool ServiceEink::deepClearScreen(int8_t temperature)
{
    EinkWaveforms_e wv = waveformSettings.mode;

    EinkPowerOn();
    changeWaveform(EinkWaveforms_e::EinkWaveformA2, temperature);

    EinkStatus_e ret;
    memset(einkRenderBuffer.get(), 15, screen.width * screen.height);
    ret = EinkUpdateFrame(
        pointTopLeft.x, pointTopLeft.y, screen.width, screen.height, einkRenderBuffer.get(), Eink4Bpp, displayMode);
    if (ret != EinkOK)
        LOG_FATAL("Failed to update frame");
    ret = EinkRefreshImage(
        pointTopLeft.x, pointTopLeft.y, screen.width, screen.height, EinkDisplayTimingsFastRefreshMode);
    if (ret != EinkOK)
        LOG_FATAL("Failed to refresh frame");

    for (uint32_t i = 0; i < 2; i++) {
        memset(einkRenderBuffer.get(), 0, screen.width * screen.height);
        ret = EinkUpdateFrame(
            pointTopLeft.x, pointTopLeft.y, screen.width, screen.height, einkRenderBuffer.get(), Eink4Bpp, displayMode);
        if (ret != EinkOK)
            LOG_FATAL("Failed to update frame");
        ret = EinkRefreshImage(
            pointTopLeft.x, pointTopLeft.y, screen.width, screen.height, EinkDisplayTimingsFastRefreshMode);
        if (ret != EinkOK)
            LOG_FATAL("Failed to refresh frame");

        memset(einkRenderBuffer.get(), 15, screen.width * screen.height);
        ret = EinkUpdateFrame(
            pointTopLeft.x, pointTopLeft.y, screen.width, screen.height, einkRenderBuffer.get(), Eink4Bpp, displayMode);
        if (ret != EinkOK)
            LOG_FATAL("Failed to update frame");
        ret = EinkRefreshImage(
            pointTopLeft.x, pointTopLeft.y, screen.width, screen.height, EinkDisplayTimingsFastRefreshMode);
        if (ret != EinkOK)
            LOG_FATAL("Failed to refresh frame");
    }

    changeWaveform(wv, temperature);

    EinkPowerOff();

    return true;
}

sys::Message_t ServiceEink::handleEinkDMATransfer(sys::Message *message)
{
    utils::time::Scoped scopedtimming("EinkDMATransfer");

    if (suspended) {
        if (suspendInProgress) {
            LOG_ERROR("drawing before suspend failed");
            suspendInProgress = false;
        }

        LOG_INFO("[ServiceEink] Received image while suspended, ignoring");
    }
    else {
        EinkPowerOn();

        int32_t temperature = EinkGetTemperatureInternal();

        EinkStatus_e ret;
        if (deepRefresh) {
            changeWaveform(EinkWaveforms_e::EinkWaveformGC16, temperature);
            EinkDitherDisplay();
        }
        else {
            changeWaveform(EinkWaveforms_e::EinkWaveformDU2, temperature);
        }

        ret = EinkUpdateFrame(
            pointTopLeft.x, pointTopLeft.y, screen.width, screen.height, einkRenderBuffer.get(), Eink4Bpp, displayMode);
        if (ret != EinkOK)
            LOG_FATAL("Failed to update frame");

        if (deepRefresh) {
            ret = EinkRefreshImage(
                pointTopLeft.x, pointTopLeft.y, screen.width, screen.height, EinkDisplayTimingsDeepCleanMode);
        }
        else {
            ret = EinkRefreshImage(
                pointTopLeft.x, pointTopLeft.y, screen.width, screen.height, EinkDisplayTimingsFastRefreshMode);
        }

        if (ret != EinkOK)
            LOG_FATAL("Failed to refresh frame");

        auto msg           = std::make_shared<service::gui::GUIDisplayReady>(suspendInProgress, shutdownInProgress);
        suspendInProgress  = false;
        shutdownInProgress = false;
        sys::Bus::SendUnicast(msg, service::name::gui, this);
    }
    return nullptr;
}

sys::Message_t ServiceEink::handleImageMessage(sys::Message *request)
{
    auto message = static_cast<service::eink::ImageMessage *>(request);

    memcpy(einkRenderBuffer.get(), message->getData(), message->getSize());
    deepRefresh = message->getDeepRefresh();

    shutdownInProgress = message->getShutdown();
    if (shutdownInProgress)
        LOG_DEBUG("Shutdown In Progress");

    suspendInProgress = message->getSuspend();
    if (suspendInProgress)
        LOG_DEBUG("Suspend In Progress");

    sys::Bus::SendUnicast(std::make_shared<service::eink::EinkDMATransfer>(), GetName(), this);
    return nullptr;
}

sys::Message_t ServiceEink::handleStateRequest(sys::Message *)
{
    return std::make_shared<service::gui::GUIDisplayReady>(suspendInProgress, shutdownInProgress);
}

sys::Message_t ServiceEink::handleTemperatureUpdate(sys::Message *)
{
    return nullptr;
}
