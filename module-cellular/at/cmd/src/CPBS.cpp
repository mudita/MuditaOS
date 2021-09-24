// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <at/cmd/CPBS.hpp>

#include <log/log.hpp>
#include <memory>
#include <string>

namespace at
{
    namespace cmd
    {
        CPBS::CPBS(at::cmd::Modifier mod) noexcept : Cmd("AT+CPBS", mod, at::default_timeout)
        {}

        CPBS::CPBS() noexcept : CPBS(at::cmd::Modifier::None)
        {}

        auto CPBS::parseCPBS(const Result &base_result) -> result::CPBS
        {

            auto constexpr responseHeader = "+CPBS: ";

            result::CPBS parsed{base_result};

            if (parsed.Result::operator bool()) {
                if (parsed.response.empty()) {
                    LOG_ERROR("Can't parse - empty response");
                    parsed.code = result::CPBS::Code::PARSING_ERROR;
                }
                else {
                    std::string str = parsed.response[0];
                    if (str.find(responseHeader) == std::string::npos) {
                        LOG_ERROR("Can't parse - bad header");
                        parsed.code = result::CPBS::Code::PARSING_ERROR;
                        return parsed;
                    }
                    utils::findAndReplaceAll(str, responseHeader, "");
                    utils::trim(str);

                    std::vector<std::string> tokens = utils::split(str, ',');

                    if (tokens.size() != cpbs::tokensCount) {
                        LOG_ERROR("Can't parse - invalid tokens count");
                        parsed.code = result::CPBS::Code::PARSING_ERROR;
                        return parsed;
                    }

                    int used  = 0;
                    int total = 0;
                    if (utils::toNumeric(tokens[static_cast<int>(cpbs::tokens::Used)], used) &&
                        utils::toNumeric(tokens[static_cast<int>(cpbs::tokens::Total)], total)) {
                        parsed.used    = used;
                        parsed.total   = total;
                        parsed.storage = tokens[static_cast<int>(cpbs::tokens::Storage)];
                    }
                    else {
                        LOG_ERROR("Can't parse - bad value");
                        parsed.code = result::CPBS::Code::PARSING_ERROR;
                    }
                }
            }
            return parsed;
        }

        void CPBS::set(const std::string &storage)
        {
            body += storage;
        }
    } // namespace cmd
    namespace result
    {
        CPBS::CPBS(const Result &that) : Result(that)
        {}

    } // namespace result
} // namespace at
