// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log.hpp>
#include <at/cmd/QNWINFO.hpp>
#include <Utils.hpp>

#include <memory>
#include <string>
#include <type_traits>

namespace at
{
    namespace cmd
    {
        QNWINFO::QNWINFO(at::cmd::Modifier mod) noexcept : Cmd("AT+QNWINFO", mod)
        {}

        QNWINFO::QNWINFO() noexcept : QNWINFO(at::cmd::Modifier::None)
        {}

        result::QNWINFO QNWINFO::parseQNWINFO(const Result &base_result)
        {
            auto baseResult = Cmd::parseBase(base_result);
            result::QNWINFO p{baseResult};
            // use parent operator bool
            if (baseResult && p.tokens.size() == 1) {
                auto tokens         = p.tokens[0];
                auto numberOfTokens = tokens.size();
                if (numberOfTokens != magic_enum::enum_count<at::result::QNWINFOTokens>()) {
                    LOG_ERROR("Can't parse - number of tokens %u is not valid",
                              static_cast<unsigned int>(p.tokens.size()));
                    p.code = result::QNWINFO::Code::PARSING_ERROR;
                    return p;
                }
                p.act = tokens[at::result::QNWINFOTokens::act];
                if (!utils::toNumeric(tokens[at::result::QNWINFOTokens::op], p.op)) {
                    p.op = 0;
                }
                p.band = tokens[at::result::QNWINFOTokens::band];
                if (!utils::toNumeric(tokens[at::result::QNWINFOTokens::channel], p.channel)) {
                    p.channel = 0;
                }
            }

            if (!p.operator bool()) {
                p.code = result::QNWINFO::Code::PARSING_ERROR;
            }

            return p;
        }
    } // namespace cmd

    namespace result
    {
        bool isStringValid(const std::string &str)
        {
            return !str.empty() && str != "\"\"";
        }

        QNWINFO::QNWINFO(const Result &that) : Result(that)
        {}

        QNWINFO::operator bool() const
        {
            if (!Result::operator bool()) {
                return false;
            }
            if (!isStringValid(act)) {
                return false;
            }
            if (!isStringValid(band)) {
                return false;
            }
            return true;
        }
    } // namespace result
} // namespace at
