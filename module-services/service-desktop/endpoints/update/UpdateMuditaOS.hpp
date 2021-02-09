// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <json/json11.hpp>
#include <module-utils/microtar/src/microtar.hpp>
#include <boot/bootconfig.hpp>
#include <purefs/filesystem_paths.hpp>

#include <cstdint>
#include <filesystem>
#include <iosfwd>
#include <string>
#include <vector>

class ServiceDesktop;
namespace fs = std::filesystem;
namespace updateos
{
    namespace file
    {
        inline constexpr auto checksums = "checksums.txt";
        inline constexpr auto sql_mig   = "sqlmig.json";
        inline constexpr auto version   = "version.json";
    } // namespace file

    namespace settings
    {
        inline constexpr auto history       = "history";
        inline constexpr auto startTime     = "startTime";
        inline constexpr auto endTime       = "endTime";
        inline constexpr auto finishedState = "finishedState";
        inline constexpr auto finishedError = "finishedError";
        inline constexpr auto fromVersion   = "fromVersion";
        inline constexpr auto toVersion     = "toVersion";
    } // namespace settings
    namespace extension
    {
        inline constexpr auto update = ".tar";
    }

    inline constexpr auto prefix_len = 8;

    enum class UpdateError
    {
        NoError,
        CantCreateTempDir,
        CantCreateUpdatesDir,
        CantRemoveUniqueTmpDir,
        CantRemoveUpdateFile,
        CantCreateUniqueTmpDir,
        CantCreateExtractedFile,
        CantOpenChecksumsFile,
        VerifyChecksumsFailure,
        VerifyVersionFailure,
        CantWriteBootloader,
        CantOpenUpdateFile,
        CantDeletePreviousOS,
        CantRenameCurrentToPrevious,
        CantRenameTempToCurrent,
        CantUpdateJSON,
        CantSaveJSON,
        CantUpdateCRC32JSON,
        CantDeltreePreviousOS,
        CantWriteToFile,
        NoBootloaderFile,
        CantOpenBootloaderFile,
        CantAllocateBuffer,
        CantLoadBootloaderFile,
        CantCopyTempToCurrent
    };

    enum class UpdateState
    {
        Initial,
        UpdateFileSet,
        CreatingDirectories,
        ExtractingFiles,
        UpdatingBootloader,
        ChecksumVerification,
        VersionVerificiation,
        ReadyForReset
    };

    enum class UpdateMessageType
    {
        UpdateFoundOnBoot,
        UpdateCheckForUpdateOnce,
        UpdateNow,
        UpdateError,
        UpdateInform
    };

    struct UpdateStats
    {
        fs::path updateFile            = "";
        fs::path fileExtracted         = "";
        fs::path updateTempDirectory   = purefs::dir::getTemporaryPath();
        uint32_t totalBytes            = 0;
        uint32_t currentExtractedBytes = 0;
        uint32_t fileExtractedSize     = 0;
        uint32_t uuid                  = 0;
        std::string messageText        = "";
        updateos::UpdateState status;
        json11::Json versionInformation;
    };

    struct UpdateRunStatus
    {
        uint32_t startTime = 0, endTime = 0;
        UpdateState finishedState = UpdateState::Initial;
        UpdateError finishedError = UpdateError::NoError;
        json11::Json fromVersion, toVersion;

        json11::Json to_json() const
        {
            return json11::Json::object{{updateos::settings::startTime, std::to_string(startTime)},
                                        {updateos::settings::endTime, std::to_string(endTime)},
                                        {updateos::settings::finishedState, (int)finishedState},
                                        {updateos::settings::finishedError, (int)finishedError},
                                        {updateos::settings::fromVersion, fromVersion},
                                        {updateos::settings::toVersion, toVersion}};
        }
    };

}; // namespace updateos

struct FileInfo
{
    FileInfo(mtar_header_t &h, unsigned long crc32);
    json11::Json to_json() const;

    std::string fileName;
    std::size_t fileSize;
    unsigned long fileCRC32;
};

class UpdateMuditaOS : public updateos::UpdateStats
{
  public:
    UpdateMuditaOS(ServiceDesktop *ownerService);

    updateos::UpdateError runUpdate();
    updateos::UpdateError prepareTempDirForUpdate(const std::filesystem::path &temporaryPath,
                                                  const std::filesystem::path &updatesOSPath);
    updateos::UpdateError unpackUpdate();
    updateos::UpdateError verifyChecksums();
    updateos::UpdateError verifyVersion();
    updateos::UpdateError updateBootloader();
    updateos::UpdateError prepareRoot();
    updateos::UpdateError updateBootJSON();
    updateos::UpdateError setUpdateFile(const std::filesystem::path &updatesOSPath, fs::path updateFileToUse);
    updateos::UpdateError cleanupAfterUpdate();
    updateos::UpdateError updateUserData();

    updateos::UpdateError informError(updateos::UpdateError errorCode, const char *format, ...);
    void informDebug(const char *format, ...);
    void informUpdate(const updateos::UpdateState statusCode, const char *format, ...);

    updateos::UpdateError writeBootloader(fs::path bootloaderFile);

    void getChecksumInfo(const std::string &infoLine, std::string &filePath, unsigned long *fileCRC32Long);
    unsigned long getExtractedFileCRC32(const std::string &filePath);
    bool unpackFileToTemp(mtar_header_t &header, unsigned long *crc32);
    const fs::path getUpdateTmpChild(const fs::path &childPath);

    static const json11::Json getVersionInfoFromFile(const fs::path &updateFile);
    static bool isUpgradeToCurrent(const std::string &versionToCompare);
    static const fs::path checkForUpdate();
    void historyValueChanged(const std::string &value);
    void setInitialHistory(const std::string &initialHistory);
    json11::Json getUpdateHistory() const
    {
        return updateHistory;
    }

  private:
    std::vector<FileInfo> filesInUpdatePackage;
    mtar_t updateTar      = {};
    ServiceDesktop *owner = nullptr;

    void storeRunStatusInDB();

    updateos::UpdateRunStatus updateRunStatus;
    json11::Json updateHistory;
    json11::Json targetVersionInfo;
    boot::BootConfig bootConfig;
    [[nodiscard]] static std::string readContent(const char *filename) noexcept;
};
