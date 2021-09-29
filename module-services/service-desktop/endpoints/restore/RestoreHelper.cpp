// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/Context.hpp>
#include <endpoints/restore/RestoreHelper.hpp>
#include <endpoints/JsonKeyNames.hpp>
#include <endpoints/message/Sender.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <service-desktop/ServiceDesktop.hpp>
#include <json11.hpp>
#include <purefs/filesystem_paths.hpp>

#include <filesystem>

namespace sdesktop::endpoints
{
    using sender::putToSendQueue;

    auto RestoreHelper::processGet(Context &context) -> ProcessResult
    {
        return checkState(context);
    }

    auto RestoreHelper::processPost(Context &context) -> ProcessResult
    {
        return executeRequest(context);
    }

    auto RestoreHelper::checkState(Context &context) -> ProcessResult
    {
        auto ownerService = static_cast<ServiceDesktop *>(owner);

        if (context.getBody()[json::taskId].is_string()) {
            if (ownerService->getBackupRestoreStatus().taskId == context.getBody()[json::taskId].string_value()) {
                context.setResponseStatus(http::Code::OK);
                context.setResponseBody(ownerService->getBackupRestoreStatus());
            }
            else {
                return {sent::no, ResponseContext{.status = http::Code::NotFound}};
            }
        }
        else if (context.getBody()[json::request].is_string()) {
            const std::string request(context.getBody()[json::request].string_value());

            if (request.compare(json::fileList)) {

                return {sent::no, ResponseContext{.status = http::Code::BadRequest}};
            }
            auto filesList = json11::Json::object{{json::files, BackupRestore::GetBackupFiles()}};
            context.setResponseBody(filesList);
        }

        LOG_DEBUG("Responding");
        putToSendQueue(context.createSimpleResponse());

        return {sent::yes, std::nullopt};
    }

    auto RestoreHelper::executeRequest(Context &context) -> ProcessResult
    {
        auto ownerService = static_cast<ServiceDesktop *>(owner);

        if (context.getBody()[json::restore].is_string()) {
            if (ownerService->getBackupRestoreStatus().state == BackupRestore::OperationState::Running) {
                LOG_WARN("Restore is running, try again later");

                return {sent::no, ResponseContext{.status = http::Code::NotAcceptable}};
            }
            else {
                const std::filesystem::path restorePoint(context.getBody()[json::restore].string_value());
                if (restorePoint.empty()) {
                    LOG_ERROR("no restorePoint in request");
                    context.setResponseBody(json11::Json::object({{json::reason, "No restore point passed"}}));

                    return {sent::no, ResponseContext{.status = http::Code::NotAcceptable}};
                }
                if (!std::filesystem::exists(purefs::dir::getBackupOSPath() / restorePoint)) {
                    LOG_ERROR("Restore point %s does not exist", (restorePoint).c_str());
                    context.setResponseBody(json11::Json::object({{json::reason, "Invalid restore point"}}));

                    return {sent::no, ResponseContext{.status = http::Code::NotFound}};
                }
                // initialize new restore information
                ownerService->prepareRestoreData(restorePoint);

                // start the request process
                ownerService->bus.sendUnicast(std::make_shared<sdesktop::RestoreMessage>(),
                                              service::name::service_desktop);

                // return new generated restore info
                context.setResponseBody(ownerService->getBackupRestoreStatus());
            }
        }
        else {
            // unknown request for backup endpoint

            return {sent::no, ResponseContext{.status = http::Code::BadRequest}};
        }

        LOG_DEBUG("Responding");
        putToSendQueue(context.createSimpleResponse());

        return {sent::yes, std::nullopt};
    }

} // namespace sdesktop::endpoints
