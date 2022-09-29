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
        context.setResponseStatus(http::Code::BadRequest);
        putToSendQueue(context.createSimpleResponse());

        return {sent::yes, std::nullopt};
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
        context.setResponseStatus(http::Code::BadRequest);
        putToSendQueue(context.createSimpleResponse());

        return {sent::yes, std::nullopt};
    }

    auto BackupHelper::executeBackupRequest([[maybe_unused]] Context &context) -> ProcessResult
    {
        if (sys::SystemManagerCommon::RebootToUpdater(owner, sys::UpdaterReason::Backup)) {
            return {sent::no, ResponseContext{.status = http::Code::NoContent}};
        }

        return {sent::no, ResponseContext{.status = http::Code::InternalServerError}};
    }

    auto BackupHelper::executeSyncRequest(Context &context) -> ProcessResult
    {
        auto ownerServicePtr = static_cast<ServiceDesktop *>(owner);

        if (ownerServicePtr->getBackupSyncRestoreStatus().state == BackupSyncRestore::OperationState::Running) {
            LOG_DEBUG("Sync already running");
            // a sync package preparation is already running, don't start a second task
            context.setResponseStatus(http::Code::NotAcceptable);
        }
        else {
            LOG_DEBUG("Starting a sync package preparation");
            // initialize new sync information
            ownerServicePtr->prepareSyncData();

            // start the sync package preparation process in the background
            ownerServicePtr->bus.sendUnicast(std::make_shared<sdesktop::SyncMessage>(), service::name::service_desktop);

            // return new generated sync package info
            context.setResponseBody(ownerServicePtr->getBackupSyncRestoreStatus());
        }

        putToSendQueue(context.createSimpleResponse());

        return {sent::yes, std::nullopt};
    }

    auto BackupHelper::checkSyncState(Context &context) -> ProcessResult
    {
        auto ownerServicePtr = static_cast<ServiceDesktop *>(owner);

        if (context.getBody()[json::taskId].is_string()) {
            if (ownerServicePtr->getBackupSyncRestoreStatus().taskId ==
                context.getBody()[json::taskId].string_value()) {
                if (ownerServicePtr->getBackupSyncRestoreStatus().state ==
                    BackupSyncRestore::OperationState::Finished) {
                    context.setResponseStatus(http::Code::SeeOther);
                }
                else {
                    context.setResponseStatus(http::Code::OK);
                }

                context.setResponseBody(ownerServicePtr->getBackupSyncRestoreStatus());
            }
            else {
                context.setResponseStatus(http::Code::NotFound);
            }
        }
        else {
            LOG_DEBUG("Backup task not found");
            context.setResponseStatus(http::Code::BadRequest);
        }

        LOG_DEBUG("Responding");
        putToSendQueue(context.createSimpleResponse());

        return {sent::yes, std::nullopt};
    }

} // namespace sdesktop::endpoints
