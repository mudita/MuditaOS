// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

extern "C"
{
#include <module-bluetooth/lib/btstack/src/bluetooth.h>
}

namespace bluetooth::sco::utils
{
    void sendZeros(hci_con_handle_t sco_handle);
}
