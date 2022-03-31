// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include <endpoints/JsonKeyNames.hpp>
#include <json11.hpp>
#include <filesystem>

namespace sys
{
    class Service;
} // namespace sys

class BackupRestore
{
  public:
    enum class Operation
    {
        Backup,
        Restore
    };

    enum class OperationState
    {
        Stopped,
        Running,
        Finished,
        Error
    };

    static const std::string opToString(const OperationState &op)
    {
        switch (op) {
        case OperationState::Stopped:
            return "stopped";
        case OperationState::Running:
            return "running";
        case OperationState::Finished:
            return "finished";
        case OperationState::Error:
            return "error";
        }
        return "unknown";
    }

    enum class CompletionCode
    {
        Success,
        VersionConflict,
        DBError,
        FSError,
        UnpackError,
        PackError,
        CopyError,
        OtherError
    };

    static const std::string completionCodeToString(const CompletionCode &code)
    {
        switch (code) {
        case CompletionCode::Success:
            return "Success";
        case CompletionCode::VersionConflict:
            return "Version conflict";
        case CompletionCode::DBError:
            return "DB operation error";
        case CompletionCode::FSError:
            return "FSError error";
        case CompletionCode::UnpackError:
            return "Backup unpack failed";
        case CompletionCode::PackError:
            return "Backup pack failed";
        case CompletionCode::CopyError:
            return "File copying failed";
        case CompletionCode::OtherError:
            return "Undetermined error";
        }
        return "unknown";
    }

    struct OperationStatus
    {
        std::filesystem::path backupTempDir;
        std::filesystem::path location;
        CompletionCode completionCode = CompletionCode::Success;
        std::string taskId;
        OperationState state = OperationState::Stopped;
        Operation operation  = Operation::Backup;
        json11::Json to_json() const
        {
            auto response = json11::Json::object{{sdesktop::endpoints::json::taskId, taskId},
                                                 {sdesktop::endpoints::json::state, opToString(state)}};

            if (state == OperationState::Error) {
                response[sdesktop::endpoints::json::reason] = completionCodeToString(completionCode);
            }

            return response;
        }
    };

    static CompletionCode BackupUserFiles(sys::Service *ownerService, std::filesystem::path &path);
    static CompletionCode RestoreUserFiles(sys::Service *ownerService, const std::filesystem::path &path);
    static json11::Json GetBackupFiles();

  private:
    static bool RemoveBackupDir(const std::filesystem::path &path);
    static bool CreateBackupDir(const std::filesystem::path &path);
    static bool PackUserFiles(const std::filesystem::path &path);
    static bool UnpackBackupFile(const std::filesystem::path &path);
    static std::string ReadVersionFromJsonFile(const std::filesystem::path &jsonFilePath);
    static bool CheckBackupVersion(const std::filesystem::path &extractedBackup);
    static bool CanRestoreFromBackup(const std::filesystem::path &path);
    static auto TempPathForBackupFile(const std::filesystem::path &tarFilePath) -> std::filesystem::path const;
    static bool ReplaceUserFiles(const std::filesystem::path &path);
    static bool WriteBackupInfo(sys::Service *ownerService, const std::filesystem::path &path);
    static std::string ReadFileAsString(const std::filesystem::path &fileToRead);
};
