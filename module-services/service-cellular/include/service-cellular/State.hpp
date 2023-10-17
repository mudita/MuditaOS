// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace sys
{
    class Service;
}

namespace cellular::service
{
    class State
    {
      public:
        enum class ST
        {
            Idle,                        /// does nothing
            WaitForStartPermission,      /// waiting for permission to start the module
            PowerUpRequest,              /// process request of power up
            StatusCheck,                 /// set on service start - check for modem status - skipped on T3 board
            PowerUpProcedure,            /// due to lack of Status pin on T3, we don't know whether is on or off
            PowerUpInProgress,           /// waiting for modem powered up by polling various bauds
            BaudDetect,                  /// baud detection procedure
            CellularConfProcedure,       /// configuration procedure
            AudioConfigurationProcedure, /// audio configuration for modem (could be in ModemConfiguration)
            CellularPrivInit,            /// init cellular internal modules
            APNConfProcedure,            /// Configure APN set by user, check if modem have similar
            SanityCheck,                 /// prior to ModemOn last sanity checks for one time configurations etc
            ModemOn,  /// modem ready - indicates that modem is fully configured, ( **SIM is not yet configured** )
            URCReady, /// State indicates that URC handling is enabled
            ModemFatalFailure, /// modem full shutdown need
            Failed,
            Ready,            /// Service is fully initialized
            PowerDownStarted, /// modem is disconnecting from network. It might take a while if poor coverage
            PowerDownWaiting, /// modem has deregistered and is now approaching low power state
            PowerDown,        /// modem is known to be turned off
        };

        enum class PowerState
        {
            Off,
            On
        };

      private:
        enum ST state = ST::Idle;
        sys::Service *owner;

      public:
        explicit State(sys::Service *owner);

        [[nodiscard]] static const char *c_str(ST state);
        [[nodiscard]] const char *c_str() const;

        /// 1. sets state of ServiceCellular
        /// 2. sends Multicast notification of ServiceCellular state
        ///
        /// \note This is for service cellular only it could be private and friend
        void set(ST state);

        ST get() const;
    };
} // namespace cellular::service
