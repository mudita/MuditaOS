// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/Context.hpp>
#include <endpoints/backup/BackupHelper.hpp>
#include <endpoints/JsonKeyNames.hpp>
#include <endpoints/message/Sender.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <service-desktop/ServiceDesktop.hpp>

#include <json11.hpp>

namespace sdesktop::endpoints
{
    using sender::putToSendQueue;

    auto BackupHelper::processGet(Context &context) -> ProcessResult
    {
        if (context.getBody()[json::messages::category].string_value() == json::messages::categorySync) {
            return checkSyncState(context);
        }

        return {sent::no, ResponseContext{.status = http::Code::BadRequest}};
    }

    auto BackupHelper::processPost(Context &context) -> ProcessResult
    {
        auto category = context.getBody()[json::messages::category].string_value();
        if (category == json::messages::categorySync) {
            return executeSyncRequest(context);
        }
        else if (category == json::messages::categoryBackup) {
            return executeBackupRequest(context);
        }

        return {sent::no, ResponseContext{.status = http::Code::BadRequest}};
    }

    auto BackupHelper::executeBackupRequest([[maybe_unused]] Context &context) -> ProcessResult
    {
        if (sys::SystemManagerCommon::RebootToRecovery(owner, sys::RecoveryReason::Backup)) {
            return {sent::no, ResponseContext{.status = http::Code::NoContent}};
        }

        return {sent::no, ResponseContext{.status = http::Code::InternalServerError}};
    }

    auto BackupHelper::executeSyncRequest([[maybe_unused]] Context &context) -> ProcessResult
    {
        auto ownerServicePtr = static_cast<ServiceDesktop *>(owner);

        if (ownerServicePtr->getSyncStatus().state == Sync::OperationState::Running) {
            LOG_DEBUG("Sync already running");
            // a sync package preparation is already running, don't start a second task
            return {sent::no, ResponseContext{.status = http::Code::NotAcceptable}};
        }
        else {
            LOG_DEBUG("Starting a sync package preparation");
            // initialize new sync information
            ownerServicePtr->prepareSyncData();

            // start the sync package preparation process in the background
            ownerServicePtr->bus.sendUnicast(std::make_shared<sdesktop::SyncMessage>(), service::name::service_desktop);

            // return new generated sync package info

            return {sent::no, ResponseContext{.status = http::Code::Accepted, .body = json11::Json::object{}}};
        }
    }

    auto BackupHelper::checkSyncState([[maybe_unused]] Context &context) -> ProcessResult
    {
        const auto ownerServicePtr = static_cast<ServiceDesktop *>(owner);
        const auto syncStatus      = ownerServicePtr->getSyncStatus();

        const auto status =
            (syncStatus.state == Sync::OperationState::Finished) ? http::Code::SeeOther : http::Code::NoContent;
        return {sent::no, ResponseContext{.status = status, .body = syncStatus}};
    }

} // namespace sdesktop::endpoints
