// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "CommandData.hpp"

namespace bluetooth
{

    class Command
    {
      public:
        enum Type
        {
            StartScan,
            StopScan,
            getDevicesAvailable,
            VisibilityOn,
            VisibilityOff,
            ConnectAudio,
            DisconnectAudio,
            PowerOn,
            PowerOff,
            Pair,
            Unpair,
            StartRinging,
            StopRinging,
            StartRouting,
            StartStream,
            StopStream,
            SwitchProfile,
            CallAnswered,
            IncomingCallNumber,
            SignalStrengthData,
            OperatorNameData,
            None,
        };

        struct CommandPack
        {
            Command::Type commandType         = Command::None;
            std::unique_ptr<CommandData> data = nullptr;
        };

        explicit Command(CommandPack &&);
        explicit Command(Command::Type type)
        {
            pack.commandType = type;
        }
        auto getType() const noexcept -> Command::Type;
        auto getData() -> DataVariant;

      private:
        CommandPack pack;
    };

} // namespace bluetooth
