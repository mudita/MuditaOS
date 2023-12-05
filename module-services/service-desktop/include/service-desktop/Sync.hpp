// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/JsonKeyNames.hpp>
#include <json11.hpp>
#include <filesystem>

namespace sys
{
    class Service;
} // namespace sys

class Sync
{
  public:
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
        DBError,
        FSError,
        PackError,
        OtherError
    };

    static const std::string completionCodeToString(const CompletionCode &code)
    {
        switch (code) {
        case CompletionCode::Success:
            return "Success";
        case CompletionCode::DBError:
            return "DB operation error";
        case CompletionCode::FSError:
            return "FSError error";
        case CompletionCode::PackError:
            return "Backup pack failed";
        case CompletionCode::OtherError:
            return "Undetermined error";
        }
        return "unknown";
    }

    struct OperationStatus
    {
        std::filesystem::path tempDir;
        CompletionCode completionCode = CompletionCode::Success;
        OperationState state = OperationState::Stopped;
        json11::Json to_json() const
        {
            auto response = json11::Json::object{{sdesktop::endpoints::json::state, opToString(state)}};

            if (state == OperationState::Error) {
                response[sdesktop::endpoints::json::reason] = completionCodeToString(completionCode);
            }

            return response;
        }
    };

    static CompletionCode PrepareSyncPackage(sys::Service *ownerService, std::filesystem::path &path);

  private:
    static bool RemoveSyncDir(const std::filesystem::path &path);
    static bool CreateSyncDir(const std::filesystem::path &path);
    static bool PackSyncFiles(const std::filesystem::path &path);
};
