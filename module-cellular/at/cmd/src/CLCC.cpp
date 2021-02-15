// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <at/cmd/CLCC.hpp>
#include <memory>
#include <string>
#include <string_view>
#include <service-cellular/CellularCall.hpp>

namespace at
{
    namespace cmd
    {
        CLCC::CLCC(at::cmd::Modifier mod) noexcept : Cmd{"AT+CLCC", mod}
        {}

        CLCC::CLCC() noexcept : CLCC{at::cmd::Modifier::None}
        {}

        auto CLCC::toBool(const std::string &text) -> bool
        {
            int ret = -1;
            if (!utils::toNumeric(text, ret) || ret == -1) {
                throw std::runtime_error("String to bool conversion failed");
            }
            return ret == 1 ? true : false;
        }

        auto CLCC::toUInt(const std::string &text) -> std::uint8_t
        {
            auto value = std::stoul(text);
            if (value > std::numeric_limits<std::uint8_t>::max()) {
                throw std::runtime_error("Out of range value");
            }
            return static_cast<std::uint8_t>(value);
        }

        template <typename T> auto CLCC::toEnum(const std::string &text) -> std::optional<T>
        {
            static_assert(std::is_enum_v<T>);
            int ret = -1;
            if (!utils::toNumeric(text, ret) || ret == -1) {
                return std::nullopt;
            }

            return magic_enum::enum_cast<T>(ret);
        }

        result::CLCC &CLCC::parse(Result &base_result)
        {
            using namespace std::literals;
            auto &baseResult = Cmd::parse(base_result);
            auto p           = new result::CLCC(baseResult);
            result           = std::unique_ptr<result::CLCC>(p);

            auto parseErrorHandler = [&p]() -> result::CLCC & {
                LOG_ERROR("Parsing error - invalid parameter passed");
                p->code = result::CLCC::Code::PARSING_ERROR;
                return *p;
            };
            if (p && p->tokens.size() > 0) {
                for (const auto &tokens : p->tokens) {
                    auto numberOfTokens = tokens.size();
                    if (numberOfTokens != 7 && numberOfTokens != 8) {
                        LOG_ERROR("Can't parse - number of tokens %u is not valid",
                                  static_cast<unsigned int>(result->tokens.size()));
                        p->code = result::CLCC::Code::PARSING_ERROR;
                        return *p;
                    }

                    try {
                        const auto callDir   = toEnum<ModemCall::CallDir>(tokens[1]);
                        const auto callState = toEnum<ModemCall::CallState>(tokens[2]);
                        const auto callMode  = toEnum<ModemCall::CallMode>(tokens[3]);
                        if (!callDir.has_value() || !callState.has_value() || !callMode.has_value()) {
                            return parseErrorHandler();
                        }
                        p->data.push_back(result::CLCC::Data{toUInt(tokens[0]),
                                                             callDir.value(),
                                                             callState.value(),
                                                             callMode.value(),
                                                             toBool(tokens[4]),
                                                             utils::PhoneNumber(tokens[5]).getView(),
                                                             tokens[6],
                                                             (numberOfTokens == 8) ? tokens[7] : "",
                                                             numberOfTokens});
                    }
                    catch (...) {
                        return parseErrorHandler();
                    }
                }
            }
            return *p;
        }
    } // namespace cmd

    namespace result
    {
        CLCC::CLCC(const Result &result) : Result{result}
        {}

    } // namespace result
} // namespace at
