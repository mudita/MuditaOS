// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "CommandData.hpp"

namespace bluetooth
{

    class OperatorNameData : public CommandData
    {
      public:
        explicit OperatorNameData(OperatorName operatorName);
        auto getData() -> DataVariant override;

      private:
        OperatorName operatorName;
    };

} // namespace bluetooth
