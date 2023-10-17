// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <at/cmd/CPBR.hpp>

#include <log/log.hpp>
#include <memory>
#include <string>

namespace at
{
    namespace cmd
    {
        CPBR::CPBR(at::cmd::Modifier mod) noexcept : Cmd("AT+CPBR", mod, at::default_timeout)
        {}

        CPBR::CPBR() noexcept : CPBR(at::cmd::Modifier::None)
        {}

        auto CPBR::parseCPBR(const Result &base_result) -> result::CPBR
        {
            auto constexpr responseHeader = "+CPBR: ";

            result::CPBR parsed{base_result};

            if (parsed.Result::operator bool()) {
                if (parsed.response.empty()) {
                    LOG_ERROR("Can't parse - empty response");
                    parsed.code = result::CPBR::Code::PARSING_ERROR;
                }
                else {

                    for (auto token : parsed.response) {

                        if (token == "OK") {
                            parsed.code = result::CPBR::Code::OK;
                            return parsed;
                        }

                        std::string str = token;
                        if (str.find(responseHeader) == std::string::npos) {
                            LOG_ERROR("Can't parse - bad header");
                            parsed.code = result::CPBR::Code::PARSING_ERROR;
                            return parsed;
                        }

                        utils::findAndReplaceAll(str, responseHeader, "");
                        utils::trim(str);

                        std::vector<std::string> tokens = utils::split(str, ',');
                        if (tokens.size() != cpbr::tokensCount) {
                            LOG_ERROR("Can't parse - invalid tokens count");
                            parsed.code = result::CPBR::Code::PARSING_ERROR;
                            return parsed;
                        }
                        int index = 0;
                        int type  = 0;
                        if (utils::toNumeric(tokens[static_cast<int>(cpbr::Tokens::Index)], index) &&
                            utils::toNumeric(tokens[static_cast<int>(cpbr::Tokens::Type)], type) &&
                            magic_enum::enum_contains<at::cpbr::ContactType>(type)) {

                            utils::findAndReplaceAll(tokens[static_cast<int>(cpbr::Tokens::Number)], "\"", "");
                            utils::findAndReplaceAll(tokens[static_cast<int>(cpbr::Tokens::Name)], "\"", "");

                            cpbr::Contact contact;
                            contact.index  = index;
                            contact.type   = static_cast<at::cpbr::ContactType>(type);
                            contact.number = tokens[static_cast<int>(cpbr::Tokens::Number)];
                            contact.name   = tokens[static_cast<int>(cpbr::Tokens::Name)];
                            parsed.contacts.push_back(contact);
                        }
                        else {
                            LOG_ERROR("Can't parse - bad value");
                            parsed.code = result::CPBR::Code::PARSING_ERROR;
                            return parsed;
                        }
                    }
                }
            }
            return parsed;
        }

        void CPBR::setSimContactsReadRange(int firstIndex, int lastIndex)
        {
            body += utils::to_string(firstIndex) + "," + utils::to_string(lastIndex);
        }
    } // namespace cmd
    namespace result
    {
        CPBR::CPBR(const Result &that) : Result(that)
        {}

    } // namespace result
} // namespace at
