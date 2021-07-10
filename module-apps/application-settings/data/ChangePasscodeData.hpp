// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

enum class ChangePasscodeAction
{
    OnlyCheckCurrentPasscode,
    OnlyProvideNewPasscode,
    ChangeCurrentPasscode
};

class ChangePasscodeData : public gui::SwitchData
{
  public:
    explicit ChangePasscodeData(ChangePasscodeAction passcodeAction) : changePasscodeAction{passcodeAction}
    {}

    const ChangePasscodeAction changePasscodeAction;
};
