// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-appmgr/data/UsbActionsParams.hpp>

using namespace app::manager::actions;

ScreenPasscodeParams::ScreenPasscodeParams(bool cancel, unsigned int _attempts, std::string _passcodeName)
    : cancel(cancel), attempts{_attempts}, passcodeName{std::move(_passcodeName)}
{}

unsigned int ScreenPasscodeParams::getAttempts() const noexcept
{
    return attempts;
}
const std::string &ScreenPasscodeParams::getPasscodeName() const noexcept
{
    return passcodeName;
}
bool ScreenPasscodeParams::isCancel() const noexcept
{
    return cancel;
}
