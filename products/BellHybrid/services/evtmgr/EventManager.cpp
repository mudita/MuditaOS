// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WorkerEvent.hpp"
#include "internal/StaticData.hpp"
#include "internal/key_sequences/KeySequenceMgr.hpp"
#include "internal/key_sequences/PowerOffSequence.hpp"
#include "internal/key_sequences/BedsideLampSequence.hpp"
#include "internal/key_sequences/FrontlightSequence.hpp"
#include "internal/key_sequences/AlarmActivateSequence.hpp"
#include "internal/key_sequences/AlarmDeactivateSequence.hpp"
#include "internal/key_sequences/ResetSequence.hpp"

#include <appmgr/messages/PowerOffPopupRequestParams.hpp>
#include <appmgr/messages/RebootPopupRequestParams.hpp>
#include <evtmgr/EventManager.hpp>
#include <evtmgr/battery/Thresholds.hpp>
#include <service-appmgr/Controller.hpp>
#include <hal/temperature_source/TemperatureSource.hpp>
#include <system/Constants.hpp>
#include <screen-light-control/ScreenLightControl.hpp>
#include <service-evtmgr/KbdMessage.hpp>
#include <service-evtmgr/ScreenLightControlMessage.hpp>
#include <service-evtmgr/WorkerEventCommon.hpp>
#include <sys/messages/AlarmActivationStatusChangeRequest.hpp>
#include <switches/LatchState.hpp>

#include <gsl/util>

namespace test
{
    std::vector<std::string> filesToCopy = {"Ancient_Greek.mp3",
                                            "Forest_Creek.mp3",
                                            "Rhythmic_Lullaby.mp3",
                                            "Autumnal_Sea.mp3",
                                            "Mountain_Lagoon.mp3",
                                            "Seaside_Symphony.mp3",
                                            "Bubbling_Brook.mp3",
                                            "Mystic_Nature.mp3",
                                            "Under_the_Water.mp3",
                                            "Charming_Bells.mp3",
                                            "Natures_Harmony.mp3",
                                            "Woodland_Ambiance.mp3"};
    std::filesystem::path proprietary() noexcept
    {
        return purefs::dir::getAssetsDirPath() / "audio";
    }
    std::filesystem::path relaxation() noexcept
    {
        return "relaxation";
    }
    std::filesystem::path destinyFolder() noexcept
    {
        return purefs::dir::getUserMediaPath() / "app/relaxation_test";
    }

    bool copyFile(std::string sourcePath, std::string destinyPath)
    {
        constexpr std::size_t buffer_size = 1024 * 1024;
        char *buffer                      = new char[buffer_size]; // 1MiB
        FILE *src_fd                      = NULL;
        FILE *dst_fd                      = NULL;
        std::size_t bytes_read;
        auto _ = gsl::finally([buffer, src_fd, dst_fd] {
            if (buffer) {
                free(buffer);
            }
            if (dst_fd) {
                std::fclose(dst_fd);
            }
            if (src_fd) {
                std::fclose(src_fd);
            }
        });
        src_fd = std::fopen(sourcePath.c_str(), "r");
        if (!src_fd) {
            LOG_ERROR("Failed to open file %s to read", sourcePath.c_str());
            return false;
        }
        dst_fd = std::fopen(destinyPath.c_str(), "w");
        if (!dst_fd) {
            LOG_ERROR("Failed to open file %s to write", destinyPath.c_str());
            return false;
        }

        if (buffer == NULL) {
            LOG_ERROR("Failed to allocate copy buffer");
            return false;
        }
        std::fseek(src_fd, 0, SEEK_END);
        std::size_t bytes_left = std::ftell(src_fd);
        std::fseek(src_fd, 0, SEEK_SET);
        do {
            bytes_read = std::fread(buffer, 1, buffer_size, src_fd);
            if (std::fwrite(buffer, sizeof(*buffer), bytes_read, dst_fd) != bytes_read) {
                LOG_ERROR("Failed to write to destination file\n");
                return false;
            }
            bytes_left -= bytes_read;
        } while (bytes_left > 0);
        return bytes_left == 0 ? true : false;
    }

    void start()
    {
        LOG_ERROR("*** test start ***");
        std::filesystem::remove_all(destinyFolder());
        std::filesystem::create_directory(destinyFolder());
        const auto startTime = xTaskGetTickCount();
        for (auto file : filesToCopy) {
            const auto sourcePath  = std::string(proprietary() / relaxation() / file);
            const auto destinyPath = std::string(destinyFolder() / file);
            LOG_ERROR("copying the file %s", file.c_str());
            const bool result = copyFile(sourcePath, destinyPath);
            if (!result) {
                LOG_ERROR("*** file copy failed !!! ***");
            }
            // test::logBuffers();
            // test::clearBuffers();
        }
        const auto endTime = xTaskGetTickCount();
        LOG_ERROR("*** test end in %ld seconds ***", (endTime - startTime) / 1000);
    }

    void vTaskCode(void *pvParameters)
    {
        configASSERT(((uint32_t)pvParameters) == 1);

        LOG_ERROR("*** test task start ***");

        for (;;) {
            /* Task code goes here. */
            if (xTaskGetTickCount() > 30000) {
                start();
            }
            vTaskDelay(10000);
        }
    }

} // namespace test

namespace
{
    auto updateTemperature = [](hal::temperature::AbstractTemperatureSource &source) {
        const auto temp = source.read();
        if (not temp) {
            LOG_FATAL("Error during reading from temperature source");
        }
        else {
            evtmgr::internal::StaticData::get().setCurrentTemperature(*temp);
        };
    };
}

EventManager::EventManager(LogDumpFunction logDumpFunction, const std::string &name)
    : EventManagerCommon(logDumpFunction,
                         {.battery{.critical = constants::criticalThreshold, .shutdown = constants::shutdownThreshold},
                          .voltage{.shutdown            = constants::shutdownVoltageThreshold,
                                   .measurementMaxCount = constants::measurementThreshold}},
                         name),
      backlightHandler(settings, this), userActivityHandler(std::make_shared<sys::CpuSentinel>(name, this), this)
{
    buildKeySequences();

#if CONFIG_ENABLE_TEMP == 1
    temperatureSource = hal::temperature::AbstractTemperatureSource::Factory::create();
    updateTemperature(*temperatureSource);
    onMinuteTick = [this](const time_t) { updateTemperature(*temperatureSource); };
#endif
}

void EventManager::handleKeyEvent(sys::Message *msg)
{
    EventManagerCommon::handleKeyEvent(msg);

    auto kbdMessage = dynamic_cast<sevm::KbdMessage *>(msg);
    if (kbdMessage == nullptr) {
        return;
    }
    if (kbdMessage->key.keyCode == bsp::KeyCodes::JoystickEnter) {
        // Power key notification
        bus.sendUnicast(std::make_shared<sevm::KbdMessage>(), service::name::system_manager);
    }

    if (kbdMessage->key.state == RawKey::State::Pressed || kbdMessage->key.state == RawKey::State::Moved) {
        userActivityHandler.handleUserInput();
        backlightHandler.handleKeyPressed(static_cast<int>(mapKey(static_cast<gui::KeyCode>(kbdMessage->key.keyCode))));
        LOG_ERROR("*** keyCode: %s ***", magic_enum::enum_name(kbdMessage->key.keyCode).data());

        // if (kbdMessage->key.keyCode == bsp::KeyCodes::JoystickDown) {
        //     test::start();
        // }
    }

    keySequenceMgr->process(kbdMessage->key);
}

EventManager::~EventManager()
{
    if (test_taskHandle) {
        /* The task was created.  Use the task's handle to delete the task. */
        vTaskDelete(test_taskHandle);
    }
}

sys::ReturnCodes EventManager::InitHandler()
{
    auto ret    = EventManagerCommon::InitHandler();
    latchStatus = bsp::bell_switches::isLatchPressed() ? sevm::LatchStatus::PRESSED : sevm::LatchStatus::RELEASED;

    /* Create the task, storing the handle. */
    BaseType_t xReturned = xTaskCreate(test::vTaskCode,   /* Function that implements the task. */
                                       "eMMC test",       /* Text name for the task. */
                                       1024 * 2,          /* Stack size in words, not bytes. */
                                       (void *)1,         /* Parameter passed into the task. */
                                       tskIDLE_PRIORITY,  /* Priority at which the task is created. */
                                       &test_taskHandle); /* Used to pass out the created task's handle. */

    if (xReturned != pdPASS) {
        LOG_ERROR("*** test task not created! ***");
    }

    return ret;
}

auto EventManager::createEventWorker() -> std::unique_ptr<WorkerEventCommon>
{
    return std::make_unique<bell::WorkerEvent>(this);
}

void EventManager::initProductEvents()
{
    backlightHandler.init();

    connect(typeid(sevm::ScreenLightControlMessage), [&](sys::Message *msgl) {
        auto *m           = static_cast<sevm::ScreenLightControlMessage *>(msgl);
        const auto params = m->getParams();
        backlightHandler.processRequest(
            m->getAction(), params.value_or(screen_light_control::Parameters()), m->getSender());
        return sys::msgHandled();
    });

    connect(typeid(sevm::ScreenLightSetAutoModeParams), [&](sys::Message *msgl) {
        auto *m = static_cast<sevm::ScreenLightSetAutoModeParams *>(msgl);
        backlightHandler.processScreenRequest(m->getAction(), screen_light_control::Parameters(m->getParams()));
        return sys::msgHandled();
    });

    connect(typeid(sevm::ScreenLightSetAutoProgressiveModeParams), [&](sys::Message *msgl) {
        auto *m = static_cast<sevm::ScreenLightSetAutoProgressiveModeParams *>(msgl);
        backlightHandler.processScreenRequest(m->getAction(), screen_light_control::Parameters(m->getParams()));
        return sys::msgHandled();
    });

    connect(typeid(sevm::ScreenLightSetManualModeParams), [&](sys::Message *msgl) {
        auto *m = static_cast<sevm::ScreenLightSetManualModeParams *>(msgl);
        backlightHandler.processScreenRequest(m->getAction(), screen_light_control::Parameters(m->getParams()));
        return sys::msgHandled();
    });

    connect(typeid(sevm::ScreenLightSetConstLinearModeParams), [&](sys::Message *msgl) {
        auto *m = static_cast<sevm::ScreenLightSetConstLinearModeParams *>(msgl);
        backlightHandler.processScreenRequest(m->getAction(), screen_light_control::Parameters(m->getParams()));
        return sys::msgHandled();
    });

    connect(sevm::ScreenLightControlRequestParameters(), [&](sys::Message *msgl) {
        screen_light_control::ManualModeParameters params = {backlightHandler.getScreenBrightnessValue()};
        auto msg = std::make_shared<sevm::ScreenLightControlParametersResponse>(
            backlightHandler.getScreenLightState(), backlightHandler.getScreenAutoModeState(), params);
        return msg;
    });

    connect(sevm::LatchStatusRequest(), [&](sys::Message *msgl) {
        auto msg = std::make_shared<sevm::LatchStatusResponse>(latchStatus);
        return msg;
    });
}

sys::MessagePointer EventManager::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{

    auto responseMessage =
        std::static_pointer_cast<sys::ResponseMessage>(EventManagerCommon::DataReceivedHandler(msgl, resp));

    if (responseMessage->retCode != sys::ReturnCodes::Unresolved) {
        return responseMessage;
    }

    return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
}
void EventManager::buildKeySequences()
{
    KeySequenceMgr::SequenceCollection collection;

    auto frontlightSeq      = std::make_unique<FrontlightSequence>(*this);
    frontlightSeq->onAction = [this]() { backlightHandler.handleScreenLight(backlight::Type::Frontlight); };
    collection.emplace_back(std::move(frontlightSeq));

    auto bedsideLampSeq      = std::make_unique<BedsideLampSequence>(*this);
    bedsideLampSeq->onAction = [this]() { backlightHandler.handleScreenLight(backlight::Type::BedsideLamp); };
    collection.emplace_back(std::move(bedsideLampSeq));

    auto powerOffSeq      = std::make_unique<PowerOffSequence>(*this);
    powerOffSeq->onAction = [this]() {
        app::manager::Controller::sendAction(
            this, app::manager::actions::ShowPopup, std::make_unique<PowerOffPopupRequestParams>());
    };
    collection.emplace_back(std::move(powerOffSeq));

    auto resetSeq      = std::make_unique<ResetSequence>(*this);
    resetSeq->onAction = [this]() {
        app::manager::Controller::sendAction(
            this, app::manager::actions::ShowPopup, std::make_unique<RebootPopupRequestParams>());
    };
    collection.emplace_back(std::move(resetSeq));

    auto alarmActivateSeq      = std::make_unique<AlarmActivateSequence>();
    alarmActivateSeq->onAction = [this]() {
        latchStatus = sevm::LatchStatus::RELEASED;
        bus.sendUnicast(
            std::make_shared<sys::AlarmActivationStatusChangeRequest>(sys::AlarmActivationStatus::ACTIVATED),
            service::name::system_manager);
    };
    collection.emplace_back(std::move(alarmActivateSeq));

    auto alarmDeactivateSeq      = std::make_unique<AlarmDeactivateSequence>();
    alarmDeactivateSeq->onAction = [this]() {
        latchStatus = sevm::LatchStatus::PRESSED;
        bus.sendUnicast(
            std::make_shared<sys::AlarmActivationStatusChangeRequest>(sys::AlarmActivationStatus::DEACTIVATED),
            service::name::system_manager);
    };
    collection.emplace_back(std::move(alarmDeactivateSeq));

    keySequenceMgr = std::make_unique<KeySequenceMgr>(std::move(collection));
}
