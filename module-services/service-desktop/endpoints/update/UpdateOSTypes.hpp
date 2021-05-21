
// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <purefs/filesystem_paths.hpp>

namespace fs = std::filesystem;

namespace fs = std::filesystem;
namespace updateos
{
    inline constexpr auto initSysVer = "0.00.0";

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
        UpdateAborted,
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
