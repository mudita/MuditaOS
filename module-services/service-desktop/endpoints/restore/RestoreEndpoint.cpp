// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/restore/RestoreEndpoint.hpp>

#include <endpoints/JsonKeyNames.hpp>
#include <endpoints/message/Sender.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <service-desktop/ServiceDesktop.hpp>
#include <endpoints/backup/BackupRestore.hpp>
#include <purefs/filesystem_paths.hpp>

#include <memory>

namespace sdesktop::endpoints
{
    using sender::putToSendQueue;

    auto RestoreEndpoint::handle(Context &context) -> void
    {
        switch (context.getMethod()) {
        case http::Method::get:
            context.setResponseBody(BackupRestore::GetBackupFiles());
            break;
        case http::Method::post:
            request(context);
            break;
        case http::Method::put:
        case http::Method::del:
            context.setResponseStatus(http::Code::BadRequest);
            break;
        }

        putToSendQueue(context.createSimpleResponse());
    }

    auto RestoreEndpoint::request(Context &context) -> sys::ReturnCodes
    {
        json11::Json responseBodyJson;
        auto owner = static_cast<ServiceDesktop *>(ownerServicePtr);

        if (context.getBody()[json::task].is_string()) {
            if (owner->getBackupRestoreStatus().task == context.getBody()[json::task].string_value()) {
                if (owner->getBackupRestoreStatus().state == ServiceDesktop::OperationState::Running) {
                    LOG_WARN("looks like a previous job is running can't start a new one");
                    context.setResponseStatus(http::Code::SeeOther);
                }

                context.setResponseBody(owner->getBackupRestoreStatus());
            }
            else {
                context.setResponseStatus(http::Code::NotFound);
            }
        }
        else if (context.getBody()[json::request] == true) {
            if (owner->getBackupRestoreStatus().state == ServiceDesktop::OperationState::Running) {
                LOG_WARN("looks like a job is running, try again later");
                context.setResponseStatus(http::Code::NotAcceptable);
            }
            else {
                const std::filesystem::path location(context.getBody()[json::location].string_value());
                if (location.empty()) {
                    LOG_ERROR("no location in request");
                    context.setResponseBody(json11::Json::object({{"msg", "no location passed"}}));
                    context.setResponseStatus(http::Code::NotAcceptable);

                    return sys::ReturnCodes::Failure;
                }
                if (!std::filesystem::exists(purefs::dir::getBackupOSPath() / location)) {
                    LOG_ERROR("file %s does not exist", (purefs::dir::getBackupOSPath() / location).c_str());
                    context.setResponseBody(json11::Json::object({{"msg", "passed location is not readable"}}));
                    context.setResponseStatus(http::Code::NotFound);

                    return sys::ReturnCodes::Failure;
                }
                // initialize new restore information
                owner->prepareRestoreData(location);

                // start the request process
                ownerServicePtr->bus.sendUnicast(std::make_shared<sdesktop::RestoreMessage>(),
                                                 service::name::service_desktop);

                // return new generated restore info
                context.setResponseBody(owner->getBackupRestoreStatus());
            }
        }
        else {
            // unknown request for backup endpoint
            context.setResponseStatus(http::Code::BadRequest);
        }

        LOG_DEBUG("responding");
        putToSendQueue(context.createSimpleResponse());

        return sys::ReturnCodes::Success;
    }

} // namespace sdesktop::endpoints
