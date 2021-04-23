// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceCellularPriv.hpp"

namespace internal
{
    ServiceCellularPriv::ServiceCellularPriv(ServiceCellular *owner)
        : owner(owner), simCard(new SimCard), state(new State(owner)){};
} // namespace internal
