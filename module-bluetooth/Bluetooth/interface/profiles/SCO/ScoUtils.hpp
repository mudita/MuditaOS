// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

extern "C"
{
#include <module-bluetooth/lib/btstack/src/bluetooth.h>
}

namespace bluetooth::sco::utils
{
    void sendZeros(hci_con_handle_t sco_handle);
}
