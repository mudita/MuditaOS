// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/BaseHelper.hpp>
#include "FileOperations.hpp"

namespace sdesktop::endpoints
{
    namespace json::fs
    {
        inline constexpr auto removeFile   = "removeFile";
        inline constexpr auto renameFile   = "renameFile";
        inline constexpr auto destFilename = "destFilename";
        inline constexpr auto listDir      = "listDir";
        inline constexpr auto path         = "path";
        inline constexpr auto fileName     = "fileName";
        inline constexpr auto fileSize     = "fileSize";
        inline constexpr auto fileCrc32    = "fileCrc32";
        inline constexpr auto chunkSize    = "chunkSize";
        inline constexpr auto chunkNo      = "chunkNo";
        inline constexpr auto data         = "data";
        inline constexpr auto rxID         = "rxID";
        inline constexpr auto txID         = "txID";

        inline constexpr auto fileDoesNotExist = "file does not exist";
    } // namespace json::fs

    class FS_Helper : public BaseHelper
    {
      public:
        explicit FS_Helper(sys::Service *p, FileOperations &fileOps) : BaseHelper(p), fileOps(fileOps)
        {}

        auto processGet(Context &context) -> ProcessResult final;
        auto processPut(Context &context) -> ProcessResult final;
        auto processDelete(Context &context) -> ProcessResult final;

      private:
        auto startGetFile(Context &context) const -> ResponseContext;
        auto getFileChunk(Context &context) const -> ResponseContext;

        auto getFreeSpaceForUserFilesMiB() const -> unsigned long;

        auto startSendFile(Context &context) const -> ResponseContext;
        auto sendFileChunk(Context &context) const -> ResponseContext;

        auto requestFileRemoval(const std::string &fileName) -> bool;
        auto requestFileRename(const std::string &fileName, const std::string &destFileName) noexcept -> bool;
        auto requestListDir(const std::string &directory) -> ResponseContext;

        auto requestLogsFlush() const -> void;

        FileOperations &fileOps;
    };
} // namespace sdesktop::endpoints
