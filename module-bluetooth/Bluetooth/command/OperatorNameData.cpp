// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OperatorNameData.hpp"

#include <utility>

namespace bluetooth
{
    OperatorNameData::OperatorNameData(OperatorName operatorName) : operatorName(std::move(operatorName))
    {}
    auto OperatorNameData::getData() -> DataVariant
    {
        return operatorName;
    }
} // namespace bluetooth
