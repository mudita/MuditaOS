// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log.hpp>
#include <memory>
#include <string>
#include <type_traits>
#include <at/cmd/CFUN.hpp>

namespace at
{
    namespace cmd
    {
        CFUN::CFUN(at::cmd::Modifier mod) noexcept : Cmd("AT+CFUN", mod, at::default_timeout)
        {}

        CFUN::CFUN() noexcept : CFUN(at::cmd::Modifier::None)
        {}

        result::CFUN &CFUN::parse(Result &base_result)
        {
            auto constexpr responseHeader = "+CFUN: ";

            auto *p = new result::CFUN(base_result);
            result  = std::unique_ptr<result::CFUN>(p);
            // use parent operator bool
            if (p->Result::operator bool()) {
                if (p->response.empty()) {
                    LOG_ERROR("Can't parse - empty response");
                    p->code = result::CFUN::Code::PARSING_ERROR;
                }
                else {
                    std::string str = p->response[0];
                    if (str.find(responseHeader) == std::string::npos) {
                        LOG_ERROR("Can't parse - bad header");
                        p->code = result::CFUN::Code::PARSING_ERROR;
                        return *p;
                    }
                    utils::findAndReplaceAll(str, responseHeader, "");
                    utils::trim(str);

                    if (int func = 0;
                        utils::toNumeric(str, func) && magic_enum::enum_contains<at::cfun::Functionality>(func)) {
                        p->functionality = static_cast<at::cfun::Functionality>(func);
                    }
                    else {
                        LOG_ERROR("Can't parse - bad value");
                        p->code = result::CFUN::Code::PARSING_ERROR;
                    }
                }
            }

            return *p;
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