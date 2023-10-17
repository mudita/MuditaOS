// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <at/cmd/QCFGUsbnet.hpp>

#include <log/log.hpp>
#include <memory>
#include <string>

namespace at
{
    namespace cmd
    {
        QCFGUsbnet::QCFGUsbnet(at::cmd::Modifier mod) noexcept : Cmd("AT+QCFG=\"usbnet\"", mod, at::default_timeout)
        {}

        QCFGUsbnet::QCFGUsbnet() noexcept : QCFGUsbnet(at::cmd::Modifier::None)
        {}

        auto QCFGUsbnet::parseQCFGUsbnet(const Result &base_result) -> result::QCFGUsbnet
        {

            auto constexpr responseHeader = "+QCFG: \"usbnet\",";

            result::QCFGUsbnet parsed{base_result};

            if (parsed) {
                if (parsed.response.empty()) {
                    LOG_ERROR("Can't parse - empty response");
                    parsed.code = result::QCFGUsbnet::Code::PARSING_ERROR;
                    return parsed;
                }

                std::string str = parsed.response[0];
                if (str.find(responseHeader) == std::string::npos) {
                    LOG_ERROR("Can't parse - bad header");
                    parsed.code = result::QCFGUsbnet::Code::PARSING_ERROR;
                    return parsed;
                }

                utils::findAndReplaceAll(str, responseHeader, "");
                utils::trim(str);
                std::vector<std::string> tokens = utils::split(str, ',');
                if (tokens.size() != qcfg::usbnet::tokensCount) {
                    LOG_ERROR("Can't parse - invalid tokens count");
                    parsed.code = result::QCFGUsbnet::Code::PARSING_ERROR;
                    return parsed;
                }

                auto net = 0;
                if (utils::toNumeric(tokens[0], net) && magic_enum::enum_contains<qcfg::usbnet::Net>(net)) {
                    parsed.net = static_cast<qcfg::usbnet::Net>(net);
                }
                else {
                    LOG_ERROR("Can't parse - bad value");
                    parsed.code = result::QCFGUsbnet::Code::PARSING_ERROR;
                    return parsed;
                }
            }
            return parsed;
        }

        void QCFGUsbnet::set(qcfg::usbnet::Net net)
        {
            body += utils::to_string(static_cast<int>(net));
        }
    } // namespace cmd
    namespace result
    {
        QCFGUsbnet::QCFGUsbnet(const Result &that) : Result(that)
        {}

    } // namespace result
} // namespace at
