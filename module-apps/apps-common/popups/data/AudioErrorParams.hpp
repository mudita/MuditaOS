// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "PopupRequestParams.hpp"

namespace gui
{
    enum class AudioErrorType
    {
        UnsupportedMediaType,
        FilesLimitExceeded,
        FileDeleted,
        Unknown
    };

    class AudioErrorParams : public PopupRequestParams
    {
      public:
        explicit AudioErrorParams(AudioErrorType errorType)
            : PopupRequestParams{popup::ID::AudioError}, errorType{errorType}
        {}

        explicit AudioErrorParams(AudioErrorParams *p) : PopupRequestParams{p->getPopupId()}, errorType(p->errorType)
        {}

        [[nodiscard]] AudioErrorType getErrorType()
        {
            return errorType;
        }

      private:
        AudioErrorType errorType;
    };
} // namespace gui
