// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "checkSmsCenter.hpp"
#include "modem/BaseChannel.hpp"
#include <at/cmd/CSCA.hpp>

[[nodiscard]] bool checkSmsCenter(at::BaseChannel &channel)
{
    auto success        = true;
    auto smsCenterCmd   = at::cmd::CSCA(at::cmd::Modifier::Get);
    auto resp           = channel.cmd(smsCenterCmd);
    auto centerResponse = smsCenterCmd.parseCSCA(resp);
    if (!centerResponse) {
        success = false;
        if (centerResponse.code == at::Result::Code::PARSING_ERROR) {
            LOG_FATAL("possibly unrecoverable SMS center value error!");
        }
    }
    return success;
}
