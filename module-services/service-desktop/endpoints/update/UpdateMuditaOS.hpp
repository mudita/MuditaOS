// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <json11.hpp>
#include <microtar.hpp>
#include <boot/bootconfig.hpp>
#include <purefs/filesystem_paths.hpp>
#include <endpoints/update/UpdateOSTypes.hpp>

#include <cstdint>
#include <filesystem>
#include <iosfwd>
#include <string>
#include <vector>
#include <atomic>

class ServiceDesktop;

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
    void informUpdateWindow();
    json11::Json getUpdateHistory() const
    {
        return updateHistory;
    }
    void setUpdateAbortFlag(bool flag)
    {
        updateAbort = flag;
    }
    bool isUpdateToBeAborted() const noexcept
    {
        return updateAbort;
    }

  private:
    std::vector<FileInfo> filesInUpdatePackage;
    mtar_t updateTar      = {};
    std::atomic_bool updateAbort = false;
    ServiceDesktop *owner = nullptr;

    void storeRunStatusInDB();

    updateos::UpdateRunStatus updateRunStatus;
    json11::Json updateHistory;
    json11::Json targetVersionInfo;
    boot::BootConfig bootConfig;
};
