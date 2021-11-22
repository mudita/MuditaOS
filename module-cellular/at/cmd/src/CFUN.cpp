// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>
#include <memory>
#include <string>
#include <type_traits>
#include <at/cmd/CFUN.hpp>
#include <chrono>

namespace at
{
    namespace cmd
    {
        using namespace std::chrono_literals;
        CFUN::CFUN(at::cmd::Modifier mod) noexcept : Cmd("AT+CFUN", mod, 17s)
        {}

        CFUN::CFUN() noexcept : CFUN(at::cmd::Modifier::None)
        {}

        result::CFUN CFUN::parseCFUN(const Result &base_result)
        {
            auto constexpr responseHeader = "+CFUN: ";

            result::CFUN p{base_result};
            // use parent operator bool
            if (p.Result::operator bool()) {
                if (p.response.empty()) {
                    LOG_ERROR("Can't parse - empty response");
                    p.code = result::CFUN::Code::PARSING_ERROR;
                }
                else {
                    std::string str = p.response[0];
                    if (str.find(responseHeader) == std::string::npos) {
                        LOG_ERROR("Can't parse - bad header");
                        p.code = result::CFUN::Code::PARSING_ERROR;
                        return p;
                    }
                    utils::findAndReplaceAll(str, responseHeader, "");
                    utils::trim(str);

                    if (int func = 0;
                        utils::toNumeric(str, func) && magic_enum::enum_contains<at::cfun::Functionality>(func)) {
                        p.functionality = static_cast<at::cfun::Functionality>(func);
                    }
                    else {
                        LOG_ERROR("Can't parse - bad value");
                        p.code = result::CFUN::Code::PARSING_ERROR;
                    }
                }
            }

            return p;
        }

        void CFUN::set(at::cfun::Functionality fuctionality, at::cfun::Reset reset)
        {
            body += utils::to_string(magic_enum::enum_integer(fuctionality));
            if (reset == cfun::Reset::ResetTheME) {
                body += "," + utils::to_string(magic_enum::enum_integer(reset));
            }
        }
    } // namespace cmd

    namespace result
    {
        CFUN::CFUN(const Result &that) : Result(that)
        {}

    } // namespace result
} // namespace at
