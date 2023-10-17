// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <SwitchData.hpp>

namespace gui
{

    enum class RelaxationErrorType
    {
        UnsupportedMediaType,
        FilesLimitExceeded,
        FileDeleted,
        Unknown
    };

    class RelaxationErrorData : public SwitchData
    {
        RelaxationErrorType errorType;

      public:
        explicit RelaxationErrorData(const RelaxationErrorType &relaxationErrorType) : errorType{relaxationErrorType}
        {}

        [[nodiscard]] RelaxationErrorType getErrorType()
        {
            return errorType;
        }
    };
} // namespace gui
