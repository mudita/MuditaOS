// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "response.hpp"
#include <Utils.hpp>
#include <magic_enum.hpp>

#include <algorithm>

namespace at
{
    namespace response
    {

        std::optional<std::string> getResponseLineATCommand(const at::Result &resp, std::string_view head)
        {
            if (resp.code == at::Result::Code::OK) {
                if (resp.response.size()) {
                    for (auto el : resp.response) {
                        if (el.compare(0, head.length(), head) == 0) {
                            auto body = utils::trim(el.substr(head.length()));
                            return body;
                        }
                    }
                }
            }
            return std::nullopt;
        }

        std::optional<std::vector<std::string>> getTokensForATCommand(const at::Result &resp, std::string_view head)
        {
            if (auto line = getResponseLineATCommand(resp, head); line) {
                const auto &commandLine = *line;
                return utils::split(commandLine, ",");
            }
            return std::nullopt;
        }
        std::optional<ResponseTokens> getTokensForATResults(const at::Result &resp, std::string_view head)
        {
            if (resp.code != at::Result::Code::OK)
                return std::nullopt;

            std::vector<std::vector<std::string>> parts;
            for (auto el : resp.response) {
                if (el.compare(0, head.length(), head) == 0) {
                    auto body = el.substr(head.length());
                    parts.push_back(utils::split(body, ","));
                }
            }

            return parts;
        }

        constexpr std::string_view AT_COPS = "+COPS:";
        bool parseCOPS(const at::Result &resp, std::vector<cops::Operator> &ret)
        {
            /// +COPS: (list of supported <stat>,long alphanumeric <oper>,
            /// short alphanumeric <oper>,numeric <oper>s)[,<Act>])s]
            ///[,,(list of supported <mode>s),(list of supported <format>s)]
            ///
            /// +COPS: (2,"PLAY","PLAY","26006",2),,(0-4),(0-2)
            /// +COPS: (2,"PLAY","PLAY","26006",2)
            /// +COPS: (2,"PLAY","PLAY","26006")
            ///
            /// In case no network, error (not empty list)

            constexpr auto minCOPSLength     = 12; ///(0,"","","")
            constexpr auto minOperatorParams = 4;
            constexpr auto maxOperatorParams = 5;

            if (auto line = getResponseLineATCommand(resp, AT_COPS); line) {
                const auto &commandLine = *line;

                if (commandLine.length() < minCOPSLength) {
                    return false;
                }
                /// separator ",," between operator list and parameters info
                auto data      = utils::split(commandLine, ",,");
                auto operators = data[0];
                if ((operators.front() == '(') && (operators.back()) == ')') {
                    operators.erase(0, 1);
                    operators.pop_back();

                    auto opArray = utils::split(operators, "),(");

                    for (auto opp : opArray) {
                        auto opParams = utils::split(opp, ",");
                        if ((opParams.size() < minOperatorParams) || (opParams.size() > maxOperatorParams))
                            return false;
                        cops::Operator op;

                        op.status = static_cast<cops::OperatorStatus>(utils::getNumericValue<int>(opParams[0]));

                        op.longName = opParams[1];
                        utils::findAndReplaceAll(op.longName, at::response::StringDelimiter, "");

                        op.shortName = opParams[2];
                        utils::findAndReplaceAll(op.shortName, at::response::StringDelimiter, "");

                        op.numericName = opParams[3];
                        utils::findAndReplaceAll(op.numericName, at::response::StringDelimiter, "");
                        if (opParams.size() == maxOperatorParams) {
                            op.technology =
                                static_cast<cops::AccessTechnology>(utils::getNumericValue<int>(opParams[4]));
                        }
                        ret.push_back(op);
                    }

                    return true;
                }
            }

            return false;
        }

        bool parseCOPS(const at::Result &resp, cops::CurrentOperatorInfo &ret)
        {
            /// ret as +COPS: <mode>[,<format>[,<oper>][,<Act>]]
            /// parameters could be 1,2,3,4 all optional in documentation !

            constexpr auto minCOPSLength = 1;

            if (auto line = getResponseLineATCommand(resp, AT_COPS); line) {
                const auto &commandLine = *line;

                if (commandLine.length() < minCOPSLength) {
                    return false;
                }

                auto opParams = utils::split(commandLine, ",");
                cops::Operator op;

                switch (opParams.size()) {
                case 4:
                    op.technology = static_cast<cops::AccessTechnology>(utils::getNumericValue<int>(opParams[3]));
                    [[fallthrough]];
                case 3: {
                    ret.setFormat(static_cast<cops::NameFormat>(utils::getNumericValue<int>(opParams[1])));
                    utils::findAndReplaceAll(opParams[2], at::response::StringDelimiter, "");
                    op.setNameByFormat(ret.getFormat(), opParams[2]);
                }
                    ret.setOperator(op);
                    [[fallthrough]];
                case 2:
                    ret.setFormat(static_cast<cops::NameFormat>(utils::getNumericValue<int>(opParams[1])));
                    [[fallthrough]];
                case 1:
                    ret.setMode(static_cast<cops::CopsMode>(utils::getNumericValue<int>(opParams[0])));
                    break;
                default:
                    return false;
                }

                return true;
            }
            return false;
        }

        bool parseQCFG_IMS(const at::Result &resp, std::pair<qcfg_ims::IMSState, qcfg_ims::VoLTEIMSState> &ret)
        {
            const std::string_view AT_QCFG_IMS = "+QCFG: \"ims\"";
            if (auto tokens = getTokensForATCommand(resp, AT_QCFG_IMS); tokens) {
                constexpr int QCFG_IMS_TokensCount = 2;
                if ((*tokens).size() == QCFG_IMS_TokensCount) {
                    ret.first  = static_cast<qcfg_ims::IMSState>(utils::getNumericValue<int>((*tokens)[0]));
                    ret.second = static_cast<qcfg_ims::VoLTEIMSState>(utils::getNumericValue<int>((*tokens)[1]));
                    return true;
                }
            }
            return false;
        }

        bool parseQPINC(const at::Result &resp, qpinc::AttemptsCounters &ret)
        {
            /// parse only first result from QPINC
            const std::string_view AT_QPINC_SC = "+QPINC:";
            if (auto tokens = getTokensForATCommand(resp, AT_QPINC_SC); tokens) {
                constexpr int QPINC_TokensCount = 3;
                auto pinc_tokens                = (*tokens);
                if (pinc_tokens.size() == QPINC_TokensCount) {
                    utils::toNumeric(pinc_tokens[1], ret.PinCounter);
                    utils::toNumeric(pinc_tokens[2], ret.PukCounter);
                    return true;
                }
            }
            return false;
        }

        bool parseCLCK(const at::Result &resp, int &ret)
        {
            const std::string_view AT_CLCK = "+CLCK:";
            if (auto tokens = getTokensForATCommand(resp, AT_CLCK); tokens) {
                if ((*tokens).size() != 0) {
                    return utils::toNumeric((*tokens)[0], ret);
                }
            }
            return false;
        }

        bool parseCSQ(std::string response, std::string &result)
        {
            std::string toErase = "+CSQ: ";
            auto pos            = response.find(toErase);
            if (pos != std::string::npos) {
                response.erase(pos, toErase.length());

                result = response;
                return true;
            }
            return false;
        }
        bool parseCSQ(std::string cellularResponse, uint32_t &result)
        {
            std::string CSQstring;
            if (parseCSQ(cellularResponse, CSQstring)) {
                auto pos = CSQstring.find(',');
                if (pos != std::string::npos) {
                    LOG_INFO("%s", CSQstring.c_str());
                    CSQstring     = CSQstring.substr(0, pos);
                    int parsedVal = 0;
                    if (utils::toNumeric(CSQstring, parsedVal) && parsedVal >= 0) {
                        result = parsedVal;
                        return true;
                    }
                }
            }
            return false;
        }
        namespace creg
        {
            bool isRegistered(uint32_t commandData)
            {

                // Creg command returns 1 when registered in home network, 5 when registered in roaming
                constexpr uint32_t registeredHome    = 1;
                constexpr uint32_t registeredRoaming = 5;

                if (commandData == registeredHome || commandData == registeredRoaming) {
                    return true;
                }
                return false;
            }
        } // namespace creg
        bool parseCREG(std::string &response, uint32_t &result)
        {
            auto resp = response;
            auto pos  = resp.find(',');
            if (pos != std::string::npos) {
                auto constexpr digitLength = 1;
                resp                       = resp.substr(pos + digitLength, digitLength);
                int parsedVal              = 0;
                if (utils::toNumeric(resp, parsedVal) && parsedVal >= 0) {
                    result = parsedVal;
                    return true;
                }
            }
            return false;
        }
        bool parseCREG(std::string &response, std::string &result)
        {
            std::map<uint32_t, std::string> cregCodes;
            cregCodes.insert(std::pair<uint32_t, std::string>(0, "Not registered"));
            cregCodes.insert(std::pair<uint32_t, std::string>(1, "Registered, home network"));
            cregCodes.insert(std::pair<uint32_t, std::string>(2, "Not registered, searching"));
            cregCodes.insert(std::pair<uint32_t, std::string>(3, "Registration denied"));
            cregCodes.insert(std::pair<uint32_t, std::string>(4, "Unknown"));
            cregCodes.insert(std::pair<uint32_t, std::string>(5, "Registered, roaming"));

            uint32_t cregValue = 0;
            if (parseCREG(response, cregValue)) {
                auto cregCode = cregCodes.find(cregValue);
                if (cregCode != cregCodes.end()) {
                    result = cregCode->second;
                    return true;
                }
            }

            return false;
        }
        bool parseQNWINFO(std::string &response, std::string &result)
        {
            std::string toErase("+QNWINFO: ");
            auto pos = response.find(toErase);
            if (pos != std::string::npos) {
                response.erase(pos, toErase.length());
                response.erase(std::remove(response.begin(), response.end(), '\"'), response.end());
                result = response;
                return true;
            }

            return false;
        }

        namespace qnwinfo
        {
            uint32_t parseNetworkFrequency(std::string &response)
            {
                auto tokens = utils::split(response, ",");

                auto constexpr qnwinfoResponseSize = 4;
                auto constexpr bandTokenPos        = 2;
                if (tokens.size() == qnwinfoResponseSize) {

                    auto constexpr lteString = "LTE";
                    if (tokens[bandTokenPos].find(gsmString) != std::string::npos ||
                        tokens[bandTokenPos].find(wcdmaString) != std::string::npos) {
                        return parseNumericBandString(tokens[bandTokenPos]);
                    }
                    else if (tokens[bandTokenPos].find(lteString) != std::string::npos) {

                        return parseLteBandString(tokens[bandTokenPos]);
                    }
                }
                return 0;
            }
            uint32_t parseNumericBandString(std::string &string)
            {
                utils::findAndReplaceAll(string, gsmString, "");
                utils::findAndReplaceAll(string, wcdmaString, "");
                utils::findAndReplaceAll(string, " ", "");
                utils::findAndReplaceAll(string, "\"", "");

                int freq = 0;
                utils::toNumeric(string, freq);
                return freq;
            }
            uint32_t parseLteBandString(std::string &string)
            {

                std::map<uint32_t, uint32_t> lteFreqs;
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_1, band_1_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_2, band_2_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_3, band_3_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_4, band_4_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_5, band_5_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_7, band_7_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_8, band_8_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_12, band_12_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_13, band_13_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_18, band_18_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_20, band_20_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_25, band_25_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_26, band_26_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_28, band_28_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_38, band_38_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_40, band_40_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_41, band_41_freq));

                auto constexpr toRemove    = "LTE BAND ";
                auto constexpr emptyString = "";
                utils::findAndReplaceAll(string, "\"", emptyString);
                utils::findAndReplaceAll(string, toRemove, emptyString);

                int band = 0;
                if (utils::toNumeric(string, band) && band < 0) {
                    return 0;
                }

                auto freq = lteFreqs.find(band);
                if (freq != lteFreqs.end()) {
                    return freq->second;
                }

                return 0;
            }
        } // namespace qnwinfo

        namespace clir
        {
            std::optional<ClirResponse> parse(const std::string &response)
            {
                auto constexpr toRemove    = "+CLIR: ";
                auto constexpr emptyString = "";

                if (response.find(toRemove) == std::string::npos) {
                    return std::nullopt;
                }

                auto resp = response;
                utils::findAndReplaceAll(resp, toRemove, emptyString);

                auto tokens = utils::split(resp, ",");
                for (auto &t : tokens) {
                    t = utils::trim(t);
                }
                if (tokens.size() == clirTokens) {
                    int state  = 0;
                    int status = 0;

                    if (!utils::toNumeric(tokens[0], state) || !utils::toNumeric(tokens[1], status)) {
                        return std::nullopt;
                    }
                    if (static_cast<unsigned int>(state) < magic_enum::enum_count<ServiceState>() &&
                        static_cast<unsigned int>(status) < magic_enum::enum_count<ServiceStatus>()) {
                        return ClirResponse(static_cast<ServiceState>(state), static_cast<ServiceStatus>(status));
                    }
                }
                return std::nullopt;
            }

            app::manager::actions::IMMICustomResultParams::MMIResultMessage getState(const ServiceState &state)
            {
                using namespace app::manager::actions;

                auto message = IMMICustomResultParams::MMIResultMessage::CommonNoMessage;
                switch (state) {
                case ServiceState::AccordingToSubscription:
                    message = IMMICustomResultParams::MMIResultMessage::ClirAccordingToSubscription;
                    break;
                case ServiceState::ServiceEnabled:
                    message = IMMICustomResultParams::MMIResultMessage::ClirEnabled;
                    break;
                case ServiceState::ServiceDisabled:
                    message = IMMICustomResultParams::MMIResultMessage::ClirDisabled;
                    break;
                }
                return message;
            }

            app::manager::actions::IMMICustomResultParams::MMIResultMessage getStatus(const ServiceStatus &status)
            {
                using namespace app::manager::actions;

                auto message = IMMICustomResultParams::MMIResultMessage::CommonNoMessage;
                switch (status) {
                case ServiceStatus::NotProvisioned:
                    message = IMMICustomResultParams::MMIResultMessage::ClirNotProvisioned;
                    break;
                case ServiceStatus::PermanentProvisioned:
                    message = IMMICustomResultParams::MMIResultMessage::ClirPermanentProvisioned;
                    break;
                case ServiceStatus::Unknown:
                    message = IMMICustomResultParams::MMIResultMessage::ClirUnknown;
                    break;
                case ServiceStatus::TemporaryRestricted:
                    message = IMMICustomResultParams::MMIResultMessage::ClirTemporaryRestricted;
                    break;
                case ServiceStatus::TemporaryAllowed:
                    message = IMMICustomResultParams::MMIResultMessage::ClirTemporaryAllowed;
                    break;
                }
                return message;
            }
        } // namespace clir

        namespace ccfc
        {

            auto parse(std::vector<std::string> response, std::vector<ParsedCcfc> &parsed) -> bool
            {

                auto constexpr toRemove    = "+CCFC: ";
                auto constexpr emptyString = "";
                auto constexpr quote       = "\"";

                parsed.clear();

                for (auto el : response) {

                    if (el.find("OK") != std::string::npos) {
                        return true;
                    }

                    if (el.find(toRemove) == std::string::npos) {
                        parsed.clear();
                        return false;
                    }

                    utils::findAndReplaceAll(el, toRemove, emptyString);
                    auto tokens = utils::split(el, ",");

                    if (tokens.size() == serviceDisabledTokenCount) {
                        parsed.push_back(ParsedCcfc(ConnectionClass::None, ForwardingStatus::NotActive, ""));
                    }
                    else if (tokens.size() > serviceDisabledTokenCount) {
                        int statusToken          = 0;
                        int connectionClassToken = 0;

                        if (!utils::toNumeric(tokens[Tokens::Status], statusToken) ||
                            !utils::toNumeric(tokens[Tokens::Class], connectionClassToken)) {
                            parsed.clear();
                            return false;
                        }
                        auto status          = static_cast<ForwardingStatus>(statusToken);
                        auto connectionClass = static_cast<ConnectionClass>(connectionClassToken);

                        if (magic_enum::enum_contains<ForwardingStatus>(status) &&
                            magic_enum::enum_contains<ConnectionClass>(connectionClass)) {
                            if (tokens[Tokens::Number].find(quote) == std::string::npos) {
                                parsed.clear();
                                return false;
                            }
                            auto number = tokens[Tokens::Number];
                            utils::findAndReplaceAll(number, quote, emptyString);
                            utils::trim(number);
                            parsed.push_back(ParsedCcfc(connectionClass, status, number));
                        }
                        else {
                            parsed.clear();
                            return false;
                        }
                    }
                }
                return true;
            }

            auto getNumbers(std::vector<ParsedCcfc> &parsed) -> CcfcNumbers
            {
                CcfcNumbers numbers;

                for (auto el : parsed) {
                    std::string number = "";
                    if (el.status == ForwardingStatus::Active) {
                        number = el.number;
                    }
                    switch (el.connectionClass) {
                    case ConnectionClass::None:
                        break;
                    case ConnectionClass::Voice:
                        numbers.voice = number;
                        break;
                    case ConnectionClass::Data:
                        break;
                    case ConnectionClass::Fax:
                        numbers.fax = number;
                        break;
                    case ConnectionClass::AllTelephonyExceptSMS:
                        break;
                    case ConnectionClass::ShortMessgeService:
                        break;
                    case ConnectionClass::DataAsync:
                        numbers.async = number;
                        break;
                    case ConnectionClass::DataSync:
                        numbers.sync = number;
                        break;
                    }
                }
                return numbers;
            }

            auto isAnyActive(std::vector<ParsedCcfc> &parsed) -> bool
            {
                for (auto el : parsed) {
                    if (el.status == ForwardingStatus::Active) {
                        return true;
                    }
                }
                return false;
            }
        } // namespace ccfc

        namespace mmi
        {
            auto getClass(const ServiceClass &serviceClass) noexcept
                -> app::manager::actions::IMMICustomResultParams::MMIResultMessage
            {
                using namespace app::manager::actions;

                auto message = IMMICustomResultParams::MMIResultMessage::CommonNoMessage;
                switch (serviceClass) {
                case ServiceClass::Voice:
                    message = IMMICustomResultParams::MMIResultMessage::CommonVoice;
                    break;
                case ServiceClass::Data:
                    message = IMMICustomResultParams::MMIResultMessage::CommonData;
                    break;
                case ServiceClass::Fax:
                    message = IMMICustomResultParams::MMIResultMessage::CommonFax;
                    break;
                case ServiceClass::DataSync:
                    message = IMMICustomResultParams::MMIResultMessage::CommonSync;
                    break;
                case ServiceClass::DataAsync:
                    message = IMMICustomResultParams::MMIResultMessage::CommonAsync;
                    break;
                case ServiceClass::AllDisabled:
                    message = IMMICustomResultParams::MMIResultMessage::CommonAllDisabled;
                    break;
                }
                return message;
            }
        } // namespace mmi

        namespace clck
        {
            auto parseQueryResponse(const std::vector<std::string> &data, std::vector<ClckParsed> &parsed) -> bool
            {
                using namespace mmi;

                auto constexpr responseToken = "+CLCK: ";
                auto constexpr emptyString   = "";
                auto constexpr minTokens     = 2;
                parsed.clear();

                for (auto el : data) {
                    if (el.find("OK") != std::string::npos) {
                        return true;
                    }
                    if (el.find(responseToken) == std::string::npos) {
                        parsed.clear();
                        return false;
                    }
                    utils::findAndReplaceAll(el, responseToken, emptyString);
                    auto tokens = utils::split(el, ",");
                    if (tokens.size() < minTokens) {
                        parsed.clear();
                        return false;
                    }
                    for (auto &t : tokens) {
                        t = utils::trim(t);
                    }

                    int statusToken       = 0;
                    int serviceClassToken = 0;

                    if (!utils::toNumeric(tokens[0], statusToken) || !utils::toNumeric(tokens[1], serviceClassToken)) {
                        return false;
                    }
                    auto status       = static_cast<Status>(statusToken);
                    auto serviceClass = static_cast<ServiceClass>(serviceClassToken);

                    if (magic_enum::enum_contains<Status>(status) &&
                        magic_enum::enum_contains<ServiceClass>(serviceClass)) {
                        parsed.push_back(ClckParsed(status, serviceClass));
                    }
                    else {
                        parsed.clear();
                        return false;
                    }
                }
                return true;
            }

            auto iterLessIter(ClckParsed a, ClckParsed b) -> bool
            {
                return a.serviceClass < b.serviceClass;
            }

            auto getStatus(const Status &status) noexcept
                -> app::manager::actions::IMMICustomResultParams::MMIResultMessage
            {
                using namespace app::manager::actions;

                auto message = IMMICustomResultParams::MMIResultMessage::CommonNoMessage;
                switch (status) {
                case Status::Disable:
                    message = IMMICustomResultParams::MMIResultMessage::CommonDeactivated;
                    break;
                case Status::Enable:
                    message = IMMICustomResultParams::MMIResultMessage::CommonActivated;
                    break;
                case Status::Query:
                    message = IMMICustomResultParams::MMIResultMessage::CommonQuery;
                    break;
                }
                return message;
            }
        } // namespace clck

        namespace clip
        {
            auto parse(std::string response) -> std::optional<ClipParsed>
            {
                auto constexpr toRemove    = "+CLIP: ";
                auto constexpr emptyString = "";

                if (response.find(toRemove) == std::string::npos) {
                    return std::nullopt;
                }

                utils::findAndReplaceAll(response, toRemove, emptyString);

                auto tokens = utils::split(response, ",");
                for (auto &t : tokens) {
                    t = utils::trim(t);
                }
                if (tokens.size() == clipTokens) {
                    int urcToken;
                    int clipToken;

                    if (!utils::toNumeric(tokens[0], urcToken) || !utils::toNumeric(tokens[1], clipToken)) {
                        return std::nullopt;
                    }
                    auto urcState  = static_cast<UrcState>(urcToken);
                    auto clipState = static_cast<ClipState>(clipToken);

                    if (magic_enum::enum_contains<UrcState>(urcState) &&
                        magic_enum::enum_contains<ClipState>(clipState)) {
                        return ClipParsed(urcState, clipState);
                    }
                }
                return std::nullopt;
            }
            auto getClipState(const ClipState &state) -> app::manager::actions::IMMICustomResultParams::MMIResultMessage
            {
                auto message = app::manager::actions::IMMICustomResultParams::MMIResultMessage::CommonNoMessage;

                switch (state) {
                case ClipState::NotProvisioned:
                    message = app::manager::actions::IMMICustomResultParams::MMIResultMessage::ClipNotProvisioned;
                    break;
                case ClipState::Provisioned:
                    message = app::manager::actions::IMMICustomResultParams::MMIResultMessage::ClipProvisioned;
                    break;
                case ClipState::Unknown:
                    message = app::manager::actions::IMMICustomResultParams::MMIResultMessage::ClipUnknown;
                    break;
                }
                return message;
            }
        } // namespace clip

        namespace ccwa
        {
            auto parse(const std::vector<std::string> &data, std::vector<CcwaParsed> &parsed) noexcept -> bool
            {

                auto constexpr toRemove    = "+CCWA: ";
                auto constexpr emptyString = "";
                auto constexpr tokenCount  = 2;
                parsed.clear();

                for (auto el : data) {

                    if (el.find("OK") != std::string::npos) {
                        return true;
                    }

                    if (el.find(toRemove) == std::string::npos) {
                        parsed.clear();
                        return false;
                    }

                    utils::findAndReplaceAll(el, toRemove, emptyString);
                    auto tokens = utils::split(el, ",");
                    if (tokens.size() != tokenCount) {
                        parsed.clear();
                        return false;
                    }

                    for (auto &t : tokens) {
                        t = utils::trim(t);
                    }
                    int statusToken       = 0;
                    int serviceClassToken = 0;

                    if (!utils::toNumeric(tokens[0], statusToken) || !utils::toNumeric(tokens[1], serviceClassToken)) {
                        parsed.clear();
                        return false;
                    }
                    auto status       = static_cast<Status>(statusToken);
                    auto serviceClass = static_cast<ServiceClass>(serviceClassToken);

                    if (magic_enum::enum_contains<Status>(status) &&
                        magic_enum::enum_contains<ServiceClass>(serviceClass)) {
                        parsed.push_back(CcwaParsed(status, serviceClass));
                    }
                    else {
                        parsed.clear();
                        return false;
                    }
                }

                return true;
            }

            auto getStatus(const Status &status) noexcept
                -> app::manager::actions::IMMICustomResultParams::MMIResultMessage
            {
                using namespace app::manager::actions;

                auto message = IMMICustomResultParams::MMIResultMessage::CommonNoMessage;
                switch (status) {
                case Status::Disable:
                    message = IMMICustomResultParams::MMIResultMessage::CallWaitingDeactivated;
                    break;
                case Status::Enable:
                    message = IMMICustomResultParams::MMIResultMessage::CallWaitingActivated;
                    break;
                }
                return message;
            }

            auto getClass(const ServiceClass &serviceClass) noexcept
                -> app::manager::actions::IMMICustomResultParams::MMIResultMessage
            {
                using namespace app::manager::actions;

                auto message = IMMICustomResultParams::MMIResultMessage::CommonNoMessage;
                switch (serviceClass) {
                case ServiceClass::Voice:
                    message = IMMICustomResultParams::MMIResultMessage::CommonVoice;
                    break;
                case ServiceClass::Data:
                    message = IMMICustomResultParams::MMIResultMessage::CommonData;
                    break;
                case ServiceClass::Fax:
                    message = IMMICustomResultParams::MMIResultMessage::CommonFax;
                    break;
                case ServiceClass::DataSync:
                    message = IMMICustomResultParams::MMIResultMessage::CommonSync;
                    break;
                case ServiceClass::DataAsync:
                    message = IMMICustomResultParams::MMIResultMessage::CommonAsync;
                    break;
                case ServiceClass::AllDisabled:
                    message = IMMICustomResultParams::MMIResultMessage::CommonAllDisabled;
                    break;
                }
                return message;
            }

        } // namespace ccwa
    }     // namespace response
} // namespace at
