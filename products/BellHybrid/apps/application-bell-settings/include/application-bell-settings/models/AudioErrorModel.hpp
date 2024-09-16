// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <memory>
#include <utf8/UTF8.hpp>
#include <popups/data/AudioErrorParams.hpp>

namespace app::bell_settings
{
    class AudioErrorModel
    {
      public:
        struct ErrorPaths
        {
            UTF8 path;
            gui::AudioErrorType type;
        };

        bool isPathOnErrorList(const UTF8 &path) const;
        void addPathToErrorList(const UTF8 &path, gui::AudioErrorType errorType);
        std::optional<gui::AudioErrorType> getErrorType(const UTF8 &path) const;

      private:
        std::vector<ErrorPaths> errorPaths;
    };
} // namespace app::bell_settings
