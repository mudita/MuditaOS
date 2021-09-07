// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log.hpp>
#include <memory>
#include <string>
#include <type_traits>
#include <at/cmd/QECCNUM.hpp>

namespace at
{
    namespace result
    {
        QECCNUM::QECCNUM(const Result &that) : Result(that)
        {}
    } // namespace result

    namespace cmd
    {

        QECCNUM::QECCNUM(Mode mode, NumberType numberType, const std::vector<std::string> &number) noexcept
            : Cmd(std::string("AT") + std::string(qeccnumCmd), at::cmd::Modifier::Set)
        {
            setRequestParameters(mode, numberType, number);
        }

        QECCNUM::QECCNUM() noexcept : Cmd(std::string("AT") + std::string(qeccnumCmd), at::cmd::Modifier::Get)
        {}

        result::QECCNUM QECCNUM::parseQECCNUM(const Result &base_result)
        {
            result::QECCNUM p{base_result};

            if (p.response.empty()) {
                p.code = result::QECCNUM::Code::PARSING_ERROR;
                return p;
            }

            bool hasData = false;
            for (auto &responseLine : p.response) {
                responseLine.erase(std::remove(responseLine.begin(), responseLine.end(), stringDelimiter),
                                   responseLine.end());
                auto commandHeader = std::string(qeccnumCmd).append(commandPostfix);

                if (responseLine.find(commandHeader) == 0) {
                    hasData     = true;
                    auto parsed = utils::split(std::string(responseLine, commandHeader.size()), dataSeparator);
                    if (parsed.size() <= 1) {
                        continue;
                    }

                    if (int category = 0; utils::toNumeric(parsed.front(), category)) {
                        const auto firstNumberPosition = 1;
                        if (category == 0) {
                            p.eccNumbersNoSim =
                                std::vector<std::string>(parsed.begin() + firstNumberPosition, parsed.end());
                        }
                        else if (category == 1) {
                            p.eccNumbersSim =
                                std::vector<std::string>(parsed.begin() + firstNumberPosition, parsed.end());
                        }
                    }
                }
            }

            if (hasData && p.eccNumbersSim.empty() && p.eccNumbersNoSim.empty()) {
                p.code = result::QECCNUM::Code::PARSING_ERROR;
            }

            return p;
        }

        void QECCNUM::setRequestParameters(Mode mode, NumberType numberType, const std::vector<std::string> &numbers)
        {
            body += utils::to_string(magic_enum::enum_integer(mode)) + "," +
                    utils::to_string(magic_enum::enum_integer(numberType));
            for (auto &number : numbers) {
                if (!number.empty()) {
                    const std::string delim = std::string(1, stringDelimiter);
                    body.append("," + delim + number + delim);
                }
            }
        }
    } // namespace cmd
} // namespace at
