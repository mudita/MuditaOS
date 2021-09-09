// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "GenericSpinner.hpp"

namespace gui
{
    using UTF8Spinner     = GenericSpinner<StringPolicy<UTF8>>;
    using UIntegerSpinner = GenericSpinner<NumericPolicy<std::uint32_t>>;
    using IntegerSpinner  = GenericSpinner<NumericPolicy<std::int32_t>>;
} // namespace gui
