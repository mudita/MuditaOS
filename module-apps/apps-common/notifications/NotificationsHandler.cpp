// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "NotificationsHandler.hpp"
#include <service-cellular/service-cellular/CellularMessage.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <service-audio/AudioMessage.hpp>
#include <service-audio/AudioServiceName.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <service-db/QueryMessage.hpp>
#include <module-db/queries/notifications/QueryNotificationsGet.hpp>
#include <Timers/TimerFactory.hpp>

#include <algorithm>
#include <cstdint>
#include <chrono>

namespace
{
    constexpr std::uint32_t dbTimeoutMs = 3000U;
    constexpr std::uint32_t defaultRepeatIntervalSec = 30U;
    constexpr std::uint32_t minRepeatIntervalSec     = 1U;
    constexpr auto vibrationInterval                 = std::chrono::milliseconds(1200);
}

using namespace notifications;

NotificationsHandler::NotificationsHandler(sys::Service *parentService, NotificationsConfiguration &notificationConfig)
    : parentService{parentService}, notificationConfig{notificationConfig}
{}
    repeatAlertTimer = sys::TimerFactory::createPeriodicTimer(parentService,
                                                              "smsEmergencyAlertRepeat",
                                                              std::chrono::seconds(defaultRepeatIntervalSec),
                                                              [this](sys::Timer &) { repeatAlertHandler(); });
    vibrationAlertTimer = sys::TimerFactory::createPeriodicTimer(parentService,
                                                                 "smsEmergencyVibrationPulse",
                                                                 vibrationInterval,
                                                                 [this](sys::Timer &) { vibrationPulseHandler(); });

void NotificationsHandler::registerMessageHandlers()
{
    parentService->connect(typeid(cellular::IncomingSMSNotificationMessage),
                           [&](sys::Message *request) -> sys::MessagePointer {
                               auto *message = static_cast<cellular::IncomingSMSNotificationMessage *>(request);
                               incomingSMSHandler(message->getNumber());
                               return sys::msgHandled();
                           });
}

void NotificationsHandler::incomingSMSHandler(const utils::PhoneNumber::View &senderNumber)
{
    auto overrideConfig = getSMSAlertOverride(senderNumber);
    const auto ignoreDnd = overrideConfig.has_value() && overrideConfig->enabled && overrideConfig->ignoreDnd;

    notificationConfig.updateCurrentSMS(currentSMSPolicy, ignoreDnd);

    if (overrideConfig.has_value() && overrideConfig->enabled) {
        if (!currentSMSPolicy.isRingtoneAllowed()) {
            stopRepeatAlertLoop();
            return;
        }
        startRepeatAlertLoop(*overrideConfig);
        return;
    }

    stopRepeatAlertLoop();
    playbackSMSRingtone(std::nullopt);
}

void NotificationsHandler::playbackSMSRingtone(std::optional<std::string> soundPathOverride)
{
    if (currentSMSPolicy.isRingtoneAllowed()) {
        const auto filePath = soundPathOverride.has_value()
                                  ? *soundPathOverride
                                  : AudioServiceAPI::GetSound(parentService, audio::PlaybackType::TextMessageRingtone);
        AudioServiceAPI::PlaybackStart(parentService, audio::PlaybackType::TextMessageRingtone, filePath);
    }
}

void NotificationsHandler::playEmergencySignal(const db::query::notifications::SMSAlertOverride &overrideConfig)
{
    if (overrideConfig.forceMaxVolume) {
        if (!activeSMSAlert.has_value() || !activeSMSAlert->volumeOverridden) {
            auto currentVolume = AudioServiceAPI::GetVolume(parentService, audio::PlaybackType::TextMessageRingtone);
            if (activeSMSAlert.has_value()) {
                activeSMSAlert->previousVolume = currentVolume;
            }
        }
        AudioServiceAPI::SetVolume(parentService, audio::maxVolume, audio::PlaybackType::TextMessageRingtone);
        if (activeSMSAlert.has_value()) {
            activeSMSAlert->volumeOverridden = true;
        }
    }

    std::optional<std::string> soundPathOverride;
    if (!overrideConfig.soundPath.empty()) {
        soundPathOverride = overrideConfig.soundPath;
    }
    playbackSMSRingtone(std::move(soundPathOverride));

}

void NotificationsHandler::startRepeatAlertLoop(const db::query::notifications::SMSAlertOverride &overrideConfig)
{
    stopRepeatAlertLoop();

    auto unreadCount = getUnreadSMSCount();
    if (!unreadCount.has_value()) {
        return;
    }

    ActiveSMSAlert alert;
    alert.overrideConfig   = overrideConfig;
    alert.initialUnreadCount = *unreadCount;
    activeSMSAlert         = std::move(alert);

    playEmergencySignal(activeSMSAlert->overrideConfig);

    if (activeSMSAlert->overrideConfig.forceVibration) {
        startContinuousVibration();
    }
    else {
        stopContinuousVibration();
    }

    const auto intervalSec = std::max(minRepeatIntervalSec,
                                      activeSMSAlert->overrideConfig.repeatIntervalSec == 0
                                          ? defaultRepeatIntervalSec
                                          : activeSMSAlert->overrideConfig.repeatIntervalSec);
    repeatAlertTimer.restart(std::chrono::seconds(intervalSec));
}

void NotificationsHandler::stopRepeatAlertLoop()
{
    repeatAlertTimer.stop();
    stopContinuousVibration();

    if (activeSMSAlert.has_value() && activeSMSAlert->volumeOverridden && activeSMSAlert->previousVolume.has_value()) {
        AudioServiceAPI::SetVolume(
            parentService, *activeSMSAlert->previousVolume, audio::PlaybackType::TextMessageRingtone);
    }
    activeSMSAlert.reset();
}

void NotificationsHandler::repeatAlertHandler()
{
    if (!activeSMSAlert.has_value()) {
        repeatAlertTimer.stop();
        return;
    }

    auto unreadCount = getUnreadSMSCount();
    if (!unreadCount.has_value() || *unreadCount < activeSMSAlert->initialUnreadCount) {
        stopRepeatAlertLoop();
        return;
    }

    const auto ignoreDnd = activeSMSAlert->overrideConfig.ignoreDnd;
    notificationConfig.updateCurrentSMS(currentSMSPolicy, ignoreDnd);

    if (!currentSMSPolicy.isRingtoneAllowed()) {
        stopRepeatAlertLoop();
        return;
    }

    playEmergencySignal(activeSMSAlert->overrideConfig);
}

void NotificationsHandler::startContinuousVibration()
{
    if (!activeSMSAlert.has_value() || !activeSMSAlert->overrideConfig.forceVibration) {
        stopContinuousVibration();
        return;
    }

    vibrationPulseHandler();
    vibrationAlertTimer.restart(vibrationInterval);
}

void NotificationsHandler::stopContinuousVibration()
{
    vibrationAlertTimer.stop();
}

void NotificationsHandler::vibrationPulseHandler()
{
    if (!activeSMSAlert.has_value() || !activeSMSAlert->overrideConfig.forceVibration) {
        stopContinuousVibration();
        return;
    }

    parentService->bus.sendUnicast(std::make_shared<SingleVibrationStart>(), service::name::audio);
}

auto NotificationsHandler::getUnreadSMSCount() const -> std::optional<std::uint32_t>
{
    auto query = std::make_unique<db::query::notifications::Get>(NotificationsRecord::Key::Sms);
    const auto [status, response] =
        DBServiceAPI::GetQueryWithReply(parentService, db::Interface::Name::Notifications, std::move(query), dbTimeoutMs);

    if (status != sys::ReturnCodes::Success || response == nullptr) {
        return std::nullopt;
    }

    auto queryResponse = dynamic_cast<db::QueryResponse *>(response.get());
    if (queryResponse == nullptr) {
        return std::nullopt;
    }

    auto result = queryResponse->getResult();
    auto *smsResult = dynamic_cast<db::query::notifications::GetResult *>(result.get());
    if (smsResult == nullptr) {
        return std::nullopt;
    }

    const auto record = smsResult->getResult();
    if (!record.isValid()) {
        return std::nullopt;
    }

    return record.value;
}

auto NotificationsHandler::getSMSAlertOverride(const utils::PhoneNumber::View &senderNumber) const
    -> std::optional<db::query::notifications::SMSAlertOverride>
{
    auto query = std::make_unique<db::query::notifications::SMSAlertOverrideGetByNumber>(senderNumber);
    const auto [status, response] =
        DBServiceAPI::GetQueryWithReply(parentService, db::Interface::Name::Notifications, std::move(query), dbTimeoutMs);

    if (status != sys::ReturnCodes::Success || response == nullptr) {
        return std::nullopt;
    }

    auto queryResponse = dynamic_cast<db::QueryResponse *>(response.get());
    if (queryResponse == nullptr) {
        return std::nullopt;
    }

    auto result = queryResponse->getResult();
    auto *overrideResult = dynamic_cast<db::query::notifications::SMSAlertOverrideGetByNumberResult *>(result.get());
    if (overrideResult == nullptr) {
        return std::nullopt;
    }

    return overrideResult->getResult();
}
