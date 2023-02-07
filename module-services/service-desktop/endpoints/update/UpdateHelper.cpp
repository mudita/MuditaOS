// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-desktop/Constants.hpp>
#include <endpoints/Context.hpp>
#include <endpoints/JsonKeyNames.hpp>
#include <endpoints/update/UpdateHelper.hpp>
#include <log/log.hpp>
#include <SystemManager/SystemManagerCommon.hpp>
#include <purefs/filesystem_paths.hpp>
#include <tar/tar.hpp>
#include <json11.hpp>
#include <md5.h>
#include <hal/boot_control.h>

#include <sys/statvfs.h>
#include <filesystem>
#include <optional>
#include <fstream>

namespace sdesktop::endpoints
{
    constexpr auto chunkSize = 1024 * 128;

    struct UpdatePackageEntries
    {
        explicit UpdatePackageEntries(const json11::Json &json)
        {
            constexpr auto recovery_token   = "recovery";
            constexpr auto bootloader_token = "bootloader";
            constexpr auto os_token         = "os";
            constexpr auto filename_token   = "filename";
            constexpr auto hash_token       = "md5sum";

            recovery   = {json[recovery_token][filename_token].string_value(),
                        json[recovery_token][hash_token].string_value()};
            bootloader = {json[bootloader_token][filename_token].string_value(),
                          json[bootloader_token][hash_token].string_value()};
            os         = {json[os_token][filename_token].string_value(), json[os_token][hash_token].string_value()};
        }

        using Entry = std::pair<std::string, std::string>;
        Entry os;
        Entry bootloader;
        Entry recovery;
    };

    std::optional<json11::Json> fetchVersionJsonFromFile(const std::filesystem::path &path)
    {
        std::ifstream versionJsonFile{path};
        if (not versionJsonFile.is_open()) {
            return std::nullopt;
        }
        std::stringstream buffer;
        buffer << versionJsonFile.rdbuf();
        std::string err;
        const auto versionJson = json11::Json::parse(buffer.str(), err);
        if (!err.empty()) {
            LOG_ERROR("Parsing '%s' failed", path.c_str());
            return std::nullopt;
        }
        return versionJson;
    }

    std::optional<UpdatePackageEntries> getUpdatePackageEntries(const std::filesystem::path &path)
    {
        if (const auto version = fetchVersionJsonFromFile(path)) {
            return UpdatePackageEntries{version.value()};
        }
        return std::nullopt;
    }

    bool validateImageEntry(const std::filesystem::path &path, const std::string &hash)
    {
        auto fd = std::fopen(path.c_str(), "rb");
        if (fd == nullptr) {
            return false;
        }

        MD5 md5;
        std::vector<std::byte> raw_data(chunkSize);
        std::size_t bytesRead;
        while ((bytesRead = std::fread(raw_data.data(), 1, chunkSize, fd)) > 0) {
            md5.add(raw_data.data(), bytesRead);
        }

        std::fclose(fd);
        return md5.getHash() == hash;
    }

    bool removeDirectory(const std::filesystem::path &path)
    {
        if (std::filesystem::is_directory(path)) {
            LOG_INFO("'%s' exists, removing", path.c_str());
            std::error_code errorCode;
            if (std::filesystem::remove_all(path, errorCode) == 0) {
                return false;
            }
        }
        return true;
    }

    bool unpackUpdatePackage(const std::filesystem::path &path, const std::filesystem::path &where)
    {
        if (not removeDirectory(where)) {
            LOG_ERROR("Removing '%s' directory failed", path.c_str());
            return false;
        }

        try {
            LOG_INFO("Unpacking '%s' to '%s'", path.c_str(), where.c_str());
            tar::unpack(path, where);
        }
        catch (const std::filesystem::filesystem_error &err) {
            LOG_ERROR("Unpacking tar '%s' failed with %s", path.c_str(), err.what());
            return false;
        }
        return true;
    }

    bool validateUpdatePackage(const std::filesystem::path &packagePath, const std::filesystem::path &binariesPath)
    {
        LOG_INFO("Validating '%s' package", packagePath.c_str());
        const auto entries = getUpdatePackageEntries(packagePath / purefs::file::version_json);
        if (not entries) {
            LOG_ERROR("Fetching package entries failed");
            return false;
        }
        const auto prefix = packagePath / binariesPath;
        return validateImageEntry(prefix / entries->os.first, entries->os.second) and
               validateImageEntry(prefix / entries->recovery.first, entries->recovery.second) and
               validateImageEntry(prefix / entries->bootloader.first, entries->bootloader.second);
    }

    bool checkAvailableSpace(const std::filesystem::path &path, const std::filesystem::path &updatePackage)
    {
        struct statvfs stat
        {};
        if (statvfs(path.c_str(), &stat) < 0) {
            LOG_ERROR("Failed to stat '%s'", path.c_str());
            return false;
        }

        const auto requiredSpace = std::filesystem::file_size(updatePackage);
        const auto freeSpace = (static_cast<std::uint64_t>(stat.f_bfree) * static_cast<std::uint64_t>(stat.f_bsize));
        LOG_INFO("Checking available space: %" PRIu64 " bytes, required: %" PRIu64 " bytes",
                 freeSpace,
                 static_cast<std::uint64_t>(requiredSpace));

        return freeSpace >= requiredSpace;
    }

    bool checkUpdatePackageFile(const std::filesystem::path &path)
    {
        LOG_INFO("Checking if update package exist, '%s'", path.c_str());
        return std::filesystem::exists(path);
    }

    void UpdateHelper::preProcess(http::Method method, Context &context)
    {
        LOG_INFO("In UpdateHelper - requesting %d", static_cast<int>(method));
    }

    auto UpdateHelper::processPost(Context &context) -> ProcessResult
    {
        const auto &body = context.getBody();

        if (body[json::update] != true || body[json::reboot] != true) {
            return {sent::no, ResponseContext{.status = http::Code::BadRequest}};
        }

        if (not checkUpdatePackageFile(purefs::dir::getTemporaryPath() / sdesktop::paths::updateFilename)) {
            return {sent::no, ResponseContext{.status = http::Code::NotFound}};
        }

        if (not checkAvailableSpace(purefs::dir::getUserDiskPath(),
                                    purefs::dir::getTemporaryPath() / sdesktop::paths::updateFilename)) {
            return {sent::no, ResponseContext{.status = http::Code::NotAcceptable}};
        }

        if (not unpackUpdatePackage(purefs::dir::getTemporaryPath() / sdesktop::paths::updateFilename,
                                    updatePackagePath)) {
            return {sent::no, ResponseContext{.status = http::Code::UnprocessableEntity}};
        }

        if (not validateUpdatePackage(updatePackagePath, binariesPath)) {
            return {sent::no, ResponseContext{.status = http::Code::UnprocessableEntity}};
        }

        if (sys::SystemManagerCommon::RebootToRecovery(owner, sys::RecoveryReason::Update)) {
            return {sent::no, ResponseContext{.status = http::Code::NoContent}};
        }

        return {sent::no, ResponseContext{.status = http::Code::InternalServerError}};
    }

    UpdateHelper::UpdateHelper(sys::Service *p)
        : BaseHelper(p), updatePackagePath{purefs::dir::getTemporaryPath() / "update"}, binariesPath{get_binary_dir()}
    {}

} // namespace sdesktop::endpoints
