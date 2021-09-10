// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/developerMode/fs/FS_Helper.hpp>

#include <log.hpp>
#include <service-desktop/Constants.hpp>
#include <service-desktop/DeveloperModeMessage.hpp>
#include <endpoints/message/Sender.hpp>

#include <filesystem>
#include <system_error>

namespace sdesktop::endpoints
{
    using sender::putToSendQueue;

    void FS_Helper::preProcess(http::Method method, Context &context)
    {
        LOG_INFO("In FS helper - requesting %d", static_cast<int>(method));
    }

    auto FS_Helper::processGet(Context &context) -> ProcessResult
    {
        const auto &body = context.getBody();
        if (body[json::fs::listDir].is_string()) {
            const auto &dir = body[json::fs::listDir].string_value();
            auto response   = requestListDir(dir);
            return {sent::no, std::move(response)};
        }
        else {
            return {sent::no, ResponseContext{.status = http::Code::BadRequest}};
        }
    }

    auto FS_Helper::processPut(Context &context) -> ProcessResult
    {
        auto body = context.getBody();
        auto code = http::Code::BadRequest;
        if (body[json::fs::removeFile].is_string()) {
            const auto &fileName = body[json::fs::removeFile].string_value();
            try {
                code = requestFileRemoval(fileName) ? http::Code::NoContent : http::Code::NotFound;
            }
            catch (const std::filesystem::filesystem_error &) {
                LOG_WARN("Can't remove requested file");
                code = http::Code::InternalServerError;
            }
        }
        else if (body[json::fs::renameFile].is_string()) {
            const auto &fileName = body[json::fs::renameFile].string_value();
            if (body[json::fs::destFileName].is_string()) {
                const auto &destFileName = body[json::fs::destFileName].string_value();
                code = requestFileRename(fileName, destFileName) ? http::Code::NoContent : http::Code::NotFound;
            }
            else {
                code = http::Code::NotFound;
            }
        }
        else {
            context.setResponseStatus(http::Code::BadRequest);
            putToSendQueue(context.createSimpleResponse());
        }

        return {sent::no, ResponseContext{.status = code}};
    }

    bool FS_Helper::requestFileRemoval(const std::string &fileName)
    {
        return std::filesystem::remove(fileName);
    }

    bool FS_Helper::requestFileRename(const std::string &fileName, const std::string &destFileName) noexcept
    {
        std::error_code ec;
        std::filesystem::rename(fileName, destFileName, ec);
        return (!ec) ? true : false;
    }

    ResponseContext FS_Helper::requestListDir(const std::string &directory)
    {
        std::vector<std::string> filesInDir;
        for (const auto &entry : std::filesystem::directory_iterator{directory}) {
            filesInDir.push_back(entry.path());
        }
        json11::Json::array jsonArr;
        jsonArr.reserve(filesInDir.size());
        for (const auto &path : filesInDir) {
            jsonArr.push_back(json11::Json::object{{json::fs::path, path}});
        }
        json11::Json::object response({{directory, jsonArr}});
        return ResponseContext{.status = http::Code::OK, .body = response};
    }

} // namespace sdesktop::endpoints
