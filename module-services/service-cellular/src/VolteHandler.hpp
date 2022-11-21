// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-cellular/VolteState.hpp>
#include <module-cellular/at/ATFactory.hpp>
#include <module-cellular/at/response.hpp>
#include <module-utils/utility/Utils.hpp>
#include <stdexcept>
#include <string_view>

namespace
{
    struct NonCopyable
    {
        explicit NonCopyable()           = default;
        ~NonCopyable()                   = default;
        NonCopyable(NonCopyable const &) = delete;
        NonCopyable &operator=(NonCopyable const &) = delete;
        NonCopyable(NonCopyable &&)                 = default;
        NonCopyable &operator=(NonCopyable &&) = default;
    };
} // namespace

namespace cellular::service
{
    using namespace at;

    struct QcfgImsResult : Result
    {
        QcfgImsResult(Result &rhs) : Result{std::move(rhs)}
        {}
    };

    template <typename CmuxChannel, typename ModemResponseParser>
    struct VolteHandler : private NonCopyable
    {
        bool switchVolte(CmuxChannel &channel, bool enable)
        {
            ModemResponseParser const parser;

            if (enable) {
                // according to Quectel, this setting doesn't have to be reset when disabling
                constexpr std::uint8_t voiceDomainPacketSwitchedPreferred = 0x03;
                auto voiceDomainAnswer =
                    channel.cmd(factory(at::AT::QNVFW) + "\"/nv/item_files/modem/mmode/voice_domain_pref\"," +
                                utils::byteToHex<std::uint8_t>(voiceDomainPacketSwitchedPreferred));
                if (!voiceDomainAnswer) {
                    throw std::runtime_error("[VoLTE] failed to set voice domain before trying to enable VoLTE");
                }

                // can be left as always on: doesn't disturb when VoLTE disabled
                auto qmbnAnswer = channel.cmd(factory(at::AT::QMBNCFG) + std::string("\"autosel\",1"));
                if (!qmbnAnswer) {
                    throw std::runtime_error("[VoLTE] failed to enable MBN auto-select before trying to enable VoLTE");
                }
            }

            auto imsCheckAnswer = channel.cmd(factory(AT::QCFG_IMS));
            if (!imsCheckAnswer) {
                throw std::runtime_error("[VoLTE] modem responded with error to QCFG_IMS");
            }

            bool alreadyConfigured;
            try {
                alreadyConfigured = parser(QcfgImsResult{imsCheckAnswer}, enable);
            }
            catch (std::runtime_error const &exc) {
                throw;
            }

            if (!alreadyConfigured) {
                using namespace response::qcfg_ims;
                auto imsToggleAnswer = channel.cmd(factory(AT::QCFG_IMS) + "," +
                                                   imsStateToString(enable ? IMSState::Enable : IMSState::Disable));
                if (!imsToggleAnswer) {
                    throw std::runtime_error("[VoLTE] failed to " + std::string(enable ? "enable" : "disable") +
                                             " IMS");
                }

                volteState = enable ? cellular::VolteState::SwitchingToOn : cellular::VolteState::SwitchingToOff;
                isFirstRunAfterSwitch = false;
            }
            else {
                if (volteState == cellular::VolteState::SwitchingToOn ||
                    volteState == cellular::VolteState::SwitchingToOff) {
                    isFirstRunAfterSwitch = true;
                }
                volteState = enable ? cellular::VolteState::On : cellular::VolteState::Off;
            }

            return alreadyConfigured;
        }

        auto getVolteState() -> cellular::VolteState
        {
            return volteState;
        }

        auto isFunctionalityResetNeeded() -> bool
        {
            return isFirstRunAfterSwitch;
        }

      private:
        std::string imsStateToString(response::qcfg_ims::IMSState imsState) const
        {
            return std::to_string(magic_enum::enum_integer(imsState));
        }

        cellular::VolteState volteState = cellular::VolteState::Undefined;
        bool isFirstRunAfterSwitch      = false;
    };
} // namespace cellular::service
