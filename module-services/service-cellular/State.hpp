#pragma once

class ServiceCellular;

namespace cellular
{
    class State
    {
      public:
        enum class ST
        {
            Idle,              /// does nothing
            PowerUpInProgress, /// set on service start - hot/cold start to CMUX (cold start && cmux reset - next state
                               /// bases on URC without much thinking
            CellularStartConfProcedure,  /// start configuration procedure
            AudioConfigurationProcedure, /// audio configuration for modem (could be in ModemConfiguration)
            SanityCheck,                 /// prior to ModemOn last sanity checks for one time configurations etc
            ModemOn, /// modem ready - indicates that modem is fully configured, ( **SIM is not yet configured** )
            ModemFatalFailure, /// modem full shutdown need
            SimSelect,         /// triggers hw SIM selection (! state now will be **changed on URC** )
            Failed,
        };

      private:
        enum ST state = ST::Idle;

      public:
        [[nodiscard]] static const char *c_str(ST state);
        /// 1. sets state of ServiceCellular
        /// 2. sends Multicast notification of ServiceCellular state
        ///
        /// \note This is for service cellular only it could be private and friend
        void set(ServiceCellular *owner, ST state);
        ST get() const;
    };
} // namespace cellular
