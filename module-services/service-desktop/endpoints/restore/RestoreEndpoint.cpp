// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/restore/RestoreEndpoint.hpp>

#include <endpoints/JsonKeyNames.hpp>
#include <endpoints/message/Sender.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <service-desktop/ServiceDesktop.hpp>
#include <service-desktop/BackupRestore.hpp>
#include <purefs/filesystem_paths.hpp>

#include <memory>

namespace sdesktop::endpoints
{
    using sender::putToSendQueue;

    auto RestoreEndpoint::handle(Context &context) -> void
    {
        switch (context.getMethod()) {
        case http::Method::get:
            handleGet(context);
            break;
        case http::Method::post:
            executeRestore(context);
            break;
        case http::Method::put:
        case http::Method::del:
            context.setResponseStatus(http::Code::BadRequest);
            break;
        }

        putToSendQueue(context.createSimpleResponse());
    }

    auto RestoreEndpoint::handleGet(Context &context) -> sys::ReturnCodes
    {
        auto owner = static_cast<ServiceDesktop *>(ownerServicePtr);

        if (context.getBody()[json::taskId].is_string()) {
            if (owner->getBackupRestoreStatus().taskId == context.getBody()[json::taskId].string_value()) {
                context.setResponseStatus(http::Code::OK);
                context.setResponseBody(owner->getBackupRestoreStatus());
            }
            else {
                context.setResponseStatus(http::Code::NotFound);
            }
        }
        else if (context.getBody()[json::request].is_string()) {
            const std::string request(context.getBody()[json::request].string_value());

            if (request.compare(json::fileList)) {
                context.setResponseStatus(http::Code::BadRequest);

                return sys::ReturnCodes::Failure;
            }
            auto filesList = json11::Json::object{{json::files, BackupRestore::GetBackupFiles()}};
            context.setResponseBody(filesList);
        }

        return sys::ReturnCodes::Success;
    }

    auto RestoreEndpoint::executeRestore(Context &context) -> sys::ReturnCodes
    {
        auto owner = static_cast<ServiceDesktop *>(ownerServicePtr);

        if (context.getBody()[json::restore].is_string()) {
            if (owner->getBackupRestoreStatus().state == BackupRestore::OperationState::Running) {
                LOG_WARN("Restore is running, try again later");
                context.setResponseStatus(http::Code::NotAcceptable);
            }
            else {
                const std::filesystem::path restorePoint(context.getBody()[json::restore].string_value());
                if (restorePoint.empty()) {
                    LOG_ERROR("no restorePoint in request");
                    context.setResponseBody(json11::Json::object({{json::reason, "No restore point passed"}}));
                    context.setResponseStatus(http::Code::NotAcceptable);

                    return sys::ReturnCodes::Failure;
                }
                if (!std::filesystem::exists(purefs::dir::getBackupOSPath() / restorePoint)) {
                    LOG_ERROR("Restore point %s does not exist", (restorePoint).c_str());
                    context.setResponseBody(json11::Json::object({{json::reason, "Invalid restore point"}}));
                    context.setResponseStatus(http::Code::NotFound);

                    return sys::ReturnCodes::Failure;
                }
                // initialize new restore information
                owner->prepareRestoreData(restorePoint);

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

        LOG_DEBUG("Responding");
        putToSendQueue(context.createSimpleResponse());

        return sys::ReturnCodes::Success;
    }

} // namespace sdesktop::endpoints
