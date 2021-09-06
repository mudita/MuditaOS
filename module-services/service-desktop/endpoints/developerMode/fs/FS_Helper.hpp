// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Context.hpp>
#include <Service/Common.hpp>
#include <endpoints/BaseHelper.hpp>

namespace parserFSM
{

    class FS_Helper : public BaseHelper
    {
      public:
        explicit FS_Helper(sys::Service *p) : BaseHelper(p)
        {}

      private:
        auto processGet(Context &context) -> ProcessResult final;
        auto processPut(Context &context) -> ProcessResult final;
        void preProcess(http::Method method, Context &context) final;

        bool requestFileRemoval(const std::string &fileName);
        bool requestFileRename(const std::string &fileName, const std::string &destFileName) noexcept;
        endpoint::ResponseContext requestListDir(const std::string &directory);
    };

    namespace json::fs
    {
        inline constexpr auto removeFile   = "removeFile";
        inline constexpr auto renameFile   = "renameFile";
        inline constexpr auto destFileName = "destfilename";
        inline constexpr auto listDir      = "listDir";
        inline constexpr auto path         = "path";
    } // namespace json::fs
} // namespace parserFSM
