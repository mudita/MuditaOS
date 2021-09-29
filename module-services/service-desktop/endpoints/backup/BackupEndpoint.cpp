// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/backup/BackupEndpoint.hpp>

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

    auto BackupEndpoint::handle(Context &context) -> void
    {
        switch (context.getMethod()) {
        case http::Method::get:
            checkState(context);
            break;
        case http::Method::post:
            executeRequest(context);
            break;
        case http::Method::put:
        case http::Method::del:
            context.setResponseStatus(http::Code::BadRequest);
            putToSendQueue(context.createSimpleResponse());
            break;
        }
    }

    auto BackupEndpoint::executeRequest(Context &context) -> sys::ReturnCodes
    {
        json11::Json responseBodyJson;
        auto owner = static_cast<ServiceDesktop *>(ownerServicePtr);

        if (owner->getBackupRestoreStatus().state == BackupRestore::OperationState::Running) {
            LOG_DEBUG("Backup already running");
            // a backup is already running, don't start a second task
            context.setResponseStatus(http::Code::NotAcceptable);
        }
        else {
            LOG_DEBUG("Starting backup");
            // initialize new backup information
            owner->prepareBackupData();

            // start the backup process in the background
            ownerServicePtr->bus.sendUnicast(std::make_shared<sdesktop::BackupMessage>(),
                                             service::name::service_desktop);

            // return new generated backup info
            context.setResponseBody(owner->getBackupRestoreStatus());
        }

        putToSendQueue(context.createSimpleResponse());

        return sys::ReturnCodes::Success;
    }

    auto BackupEndpoint::checkState(Context &context) -> sys::ReturnCodes
    {
        json11::Json responseBodyJson;
        auto owner = static_cast<ServiceDesktop *>(ownerServicePtr);

        if (context.getBody()[json::taskId].is_string()) {
            if (owner->getBackupRestoreStatus().taskId == context.getBody()[json::taskId].string_value()) {
                if (owner->getBackupRestoreStatus().state == BackupRestore::OperationState::Finished) {
                    context.setResponseStatus(http::Code::SeeOther);
                }
                else {
                    context.setResponseStatus(http::Code::OK);
                }

                context.setResponseBody(owner->getBackupRestoreStatus());
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

        return sys::ReturnCodes::Success;
    }

} // namespace sdesktop::endpoints
