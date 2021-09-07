// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log.hpp>
#include <memory>
#include <string>
#include <type_traits>
#include <at/cmd/CSCA.hpp>

namespace at
{
    namespace cmd
    {
        CSCA::CSCA(at::cmd::Modifier mod) noexcept : Cmd("AT+CSCA", mod)
        {}

        CSCA::CSCA() noexcept : CSCA(at::cmd::Modifier::None)
        {}

        result::CSCA CSCA::parseCSCA(const Result &base_result)
        {
            result::CSCA p{base_result};
            // use parent operator bool
            if (p.Result::operator bool()) {
                if (p.response.empty()) {
                    LOG_ERROR("Can't parse - empty response");
                    p.code = result::CSCA::Code::PARSING_ERROR;
                }
                else {
                    auto str = p.response[0];
                    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
                    auto pos           = str.find(":");
                    auto end           = str.find(",");
                    p.smsCenterAddress = std::string(str, pos + 1, end - pos - 1);
                    p.smsTypeOfAddress = std::string(str, end + 1);
                    if (p.smsCenterAddress.empty() || p.smsTypeOfAddress.empty()) {
                        LOG_ERROR("Can't parse - bad value");
                        p.code = result::CSCA::Code::PARSING_ERROR;
                    }
                }
            }
            return p;
        }

        void CSCA::set(const utils::PhoneNumber::View &smsCenterAddress, int smsTypeOfAddress)
        {
            body += "\"" + smsCenterAddress.getE164() + "\"," + std::to_string(smsTypeOfAddress);
        }
    } // namespace cmd

    namespace result
    {
        CSCA::CSCA(const Result &that) : Result(that)
        {}

        CSCA::operator bool() const
        {
            if (!Result::operator bool()) {
                return false;
            }
            if (smsCenterAddress.empty() || smsCenterAddress == "\"\"") {
                return false;
            }
            if (smsTypeOfAddress.empty() || smsTypeOfAddress == "\"\"") {
                return false;
            }
            return true;
        }
    } // namespace result
} // namespace at
