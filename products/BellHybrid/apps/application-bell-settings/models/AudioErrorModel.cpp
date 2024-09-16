// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "models/AudioErrorModel.hpp"

namespace app::bell_settings
{
    bool AudioErrorModel::isPathOnErrorList(const UTF8 &path) const
    {
        const auto res =
            std::find_if(errorPaths.begin(), errorPaths.end(), [&path](auto err) { return err.path == path; });
        return res != errorPaths.end();
    }

    void AudioErrorModel::addPathToErrorList(const UTF8 &path, gui::AudioErrorType errorType)
    {
        errorPaths.emplace_back(ErrorPaths{path, errorType});
    }

    std::optional<gui::AudioErrorType> AudioErrorModel::getErrorType(const UTF8 &path) const
    {
        const auto res =
            std::find_if(errorPaths.begin(), errorPaths.end(), [&path](auto err) { return err.path == path; });
        if (res != errorPaths.end()) {
            return (*res).type;
        }
        return std::nullopt;
    }
} // namespace app::bell_settings
