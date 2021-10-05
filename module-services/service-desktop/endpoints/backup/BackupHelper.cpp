// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/Context.hpp>
#include <endpoints/backup/BackupHelper.hpp>
#include <endpoints/JsonKeyNames.hpp>
#include <endpoints/message/Sender.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <service-desktop/ServiceDesktop.hpp>

#include <json11.hpp>
#include <purefs/filesystem_paths.hpp>

#include <filesystem>
#include <memory>

namespace sdesktop::endpoints
{
    using sender::putToSendQueue;

    auto BackupHelper::processGet(Context &context) -> ProcessResult
    {
        return checkState(context);
    }

    auto BackupHelper::processPost(Context &context) -> ProcessResult
    {
        return executeRequest(context);
    }

    auto BackupHelper::executeRequest(Context &context) -> ProcessResult
    {
        auto ownerServicePtr = static_cast<ServiceDesktop *>(owner);

        if (ownerServicePtr->getBackupRestoreStatus().state == BackupRestore::OperationState::Running) {
            LOG_DEBUG("Backup already running");
            // a backup is already running, don't start a second task
            context.setResponseStatus(http::Code::NotAcceptable);
        }
        else {
            LOG_DEBUG("Starting backup");
            // initialize new backup information
            ownerServicePtr->prepareBackupData();

            // start the backup process in the background
            ownerServicePtr->bus.sendUnicast(std::make_shared<sdesktop::BackupMessage>(),
                                             service::name::service_desktop);

            // return new generated backup info
            context.setResponseBody(ownerServicePtr->getBackupRestoreStatus());
        }

        putToSendQueue(context.createSimpleResponse());

        return {sent::yes, std::nullopt};
    }

    auto BackupHelper::checkState(Context &context) -> ProcessResult
    {
        auto ownerServicePtr = static_cast<ServiceDesktop *>(owner);

        if (context.getBody()[json::taskId].is_string()) {
            if (ownerServicePtr->getBackupRestoreStatus().taskId == context.getBody()[json::taskId].string_value()) {
                if (ownerServicePtr->getBackupRestoreStatus().state == BackupRestore::OperationState::Finished) {
                    context.setResponseStatus(http::Code::SeeOther);
                }
                else {
                    context.setResponseStatus(http::Code::OK);
                }

                context.setResponseBody(ownerServicePtr->getBackupRestoreStatus());
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
