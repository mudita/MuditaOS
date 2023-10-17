// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>
#include <memory>
#include <string>
#include <type_traits>
#include <at/cmd/CSQ.hpp>
#include <chrono>

namespace at
{
    namespace cmd
    {
        using namespace std::chrono_literals;
        CSQ::CSQ(at::cmd::Modifier mod) noexcept : Cmd("AT+CSQ", mod, 300ms)
        {}

        CSQ::CSQ() noexcept : CSQ(at::cmd::Modifier::None)
        {}

        result::CSQ CSQ::parseCSQ(const Result &base_result)
        {
            auto constexpr responseHeader = "+CSQ: ";

            result::CSQ p{base_result};
            // use parent operator bool
            if (p.Result::operator bool()) {
                if (p.response.empty()) {
                    LOG_ERROR("Can't parse - empty response");
                    p.code = result::CSQ::Code::PARSING_ERROR;

                    return p;
                }

                std::string str = p.response[0];
                if (str.find(responseHeader) == std::string::npos) {
                    LOG_ERROR("Can't parse - bad header");
                    p.code = result::CSQ::Code::PARSING_ERROR;
                    return p;
                }

                utils::findAndReplaceAll(str, responseHeader, "");
                utils::trim(str);

                std::vector<std::string> tokens = utils::split(str, ',');
                if (tokens.size() != csq::tokensCount) {
                    LOG_ERROR("Can't parse - wrong token count");
                    p.code = result::CSQ::Code::PARSING_ERROR;
                    return p;
                }

                auto csq = 0;
                auto ber = 0;
                if (utils::toNumeric(tokens.at(magic_enum::enum_integer(csq::Tokens::Csq)), csq) &&
                    utils::toNumeric(tokens.at(magic_enum::enum_integer(csq::Tokens::Ber)), ber)) {
                    p.csq = csq;
                    p.ber = ber;
                }
                else {
                    LOG_ERROR("Can't parse - bad value");
                    p.code = result::CSQ::Code::PARSING_ERROR;
                }
            }

            return p;
        }

    } // namespace cmd

    namespace result
    {
        CSQ::CSQ(const Result &that) : Result(that)
        {}

    } // namespace result
} // namespace at
