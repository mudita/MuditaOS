// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <at/cmd/QSIMSTAT.hpp>

#include <log.hpp>
#include <memory>
#include <string>

namespace at
{
    namespace cmd
    {
        QSIMSTAT::QSIMSTAT(at::cmd::Modifier mod) noexcept : Cmd("AT+QSIMSTAT", mod, at::default_timeout)
        {}

        QSIMSTAT::QSIMSTAT() noexcept : QSIMSTAT(at::cmd::Modifier::None)
        {}

        auto QSIMSTAT::parseQSIMSTAT(const Result &base_result) -> result::QSIMSTAT
        {

            auto constexpr responseHeader      = "+QSIMSTAT: ";
            auto constexpr qsimstatTokensCount = 2;
            result::QSIMSTAT parsed{base_result};

            if (parsed) {
                if (parsed.response.empty()) {
                    LOG_ERROR("Can't parse - empty response");
                    parsed.code = result::QSIMSTAT::Code::PARSING_ERROR;
                }
                else {
                    std::string str = parsed.response[0];
                    if (str.find(responseHeader) == std::string::npos) {
                        LOG_ERROR("Can't parse - bad header");
                        parsed.code = result::QSIMSTAT::Code::PARSING_ERROR;
                        return parsed;
                    }

                    utils::findAndReplaceAll(str, responseHeader, "");
                    utils::trim(str);

                    std::vector<std::string> tokens = utils::split(str, ',');

                    if (tokens.size() != qsimstatTokensCount) {
                        LOG_ERROR("Can't parse - invalid tokens count");
                        parsed.code = result::QSIMSTAT::Code::PARSING_ERROR;
                        return parsed;
                    }

                    parseTokens(tokens, parsed);
                }
            }
            return parsed;
        }

        void QSIMSTAT::parseTokens(const std::vector<std::string> &tokens, result::QSIMSTAT &parsed)
        {
            auto status = 0, enabled = 0;
            if (utils::toNumeric(tokens[magic_enum::enum_integer(responseTokens::SimInserted)], status) &&
                utils::toNumeric(tokens[magic_enum::enum_integer(responseTokens::Enabled)], enabled)) {
                if (magic_enum::enum_contains<SimInsertedStatus>(status) &&
                    magic_enum::enum_contains<at::SimInsertedStatusEnable>(enabled)) {
                    parsed.enabled = static_cast<at::SimInsertedStatusEnable>(enabled);
                    parsed.status  = static_cast<SimInsertedStatus>(status);
                    return;
                }
            }

            LOG_ERROR("Can't parse - bad value");
            parsed.code = result::QSIMSTAT::Code::PARSING_ERROR;
        }
    } // namespace cmd
    namespace result
    {
        QSIMSTAT::QSIMSTAT(const Result &that) : Result(that)
        {}

    } // namespace result
} // namespace at
