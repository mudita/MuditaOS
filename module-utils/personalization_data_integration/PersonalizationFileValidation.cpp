// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-utils/gsl/gsl>
#include "module-utils/bootconfig/src/bootconfig.cpp"
#include "PersonalizationFileValidation.hpp"

namespace phone_personalization
{

    auto PersonalizationFileParser::loadFileContent() const -> std::string
    {
        std::string content;
        std::ifstream in(filePath);
        if (!in.is_open()) {
            LOG_FATAL("Error while loading personalization file");
            return "";
        }
        std::getline(in, content, std::string::traits_type::to_char_type(std::string::traits_type::eof()));
        return content;
    }

    auto PersonalizationFileParser::parseJson() const -> json11::Json
    {

        auto content = loadFileContent();

        std::string parseErrors;
        LOG_INFO("parsed %s: \"%s\"", filePath.c_str(), content.c_str());

        auto jsonObj = json11::Json::parse(content, parseErrors);

        if (parseErrors.length() != 0) {
            LOG_FATAL("JSON format error: %s", parseErrors.c_str());
            return nullptr;
        }

        return jsonObj;
    }

    void PersonalizationFileValidator::validateByFormat(const std::string &key)
    {
        auto paramObj = jsonObj[key];
        if (paramObj != nullptr) {
            auto param = paramObj.string_value();
            if (param.empty()) {
                LOG_ERROR("Parameter: '%s' is epmty!", param.c_str());
                paramsModel[key].isValid = false;
                return;
            }
            if (param.find(param::serial_number::prefix) != 0) {
                LOG_ERROR("Wrong format of parameter: '%s'", key.c_str());
                paramsModel[key].isValid = false;
                return;
            }
            if (param.size() != param::serial_number::length) {
                LOG_ERROR("Parameter: '%s' has wrong size", key.c_str());
                paramsModel[key].isValid = false;
                return;
            }
            LOG_INFO("Valid parameter: %s with value: %s", key.c_str(), param.c_str());
            paramsModel[key].isValid = true;
            return;
        }

        LOG_ERROR("Parameter does not exist in json object!");
        paramsModel[key].isValid = false;
    }

    void PersonalizationFileValidator::validateByValues(const std::string &key)
    {
        if (jsonObj != nullptr) {
            auto result = std::find(std::begin(paramsModel[key].validValues),
                                    std::end(paramsModel[key].validValues),
                                    jsonObj[key].string_value());

            if (result == std::end(paramsModel[key].validValues)) {
                LOG_ERROR("Parameter: '%s' is invalid!", key.c_str());
                paramsModel[key].isValid = false;
            }
            else {
                LOG_INFO("Parameter: '%s' is valid", key.c_str());
                paramsModel[key].isValid = true;
            }
            return;
        }
        LOG_ERROR("Params json object not parsed!");
    }

    auto PersonalizationFileValidator::validateFileCRC() -> bool
    {
        if (!boot::readAndVerifyCRC(filePath)) {
            LOG_ERROR("Invalid crc calculation!");
            return false;
        }

        LOG_INFO("CRC is correct");
        return true;
    }

    auto PersonalizationFileValidator::validateJsonObject() -> bool
    {
        if (jsonObj == nullptr || jsonObj == json11::Json()) {
            return false;
        }
        else {
            return true;
        }
    }

    auto PersonalizationFileValidator::validateParameters() -> bool
    {
        for (const auto &param : paramsModel) {
            param.second.validateCallback();
            if (!param.second.isValid && param.second.mandatory == MandatoryParameter::yes) {
                LOG_FATAL("Mandatory parameter: %s  is invalid", param.first.c_str());
                return false;
            }
        }
        return true;
    }

    auto PersonalizationFileValidator::validate() -> bool
    {
        if (!validateFileCRC()) {
            return false;
        }
        if (!validateJsonObject()) {
            return false;
        }
        if (!validateParameters()) {
            return false;
        }

        return true;
    }

    void PersonalizationData::setParamsFromJson(const json11::Json &jsonObj)
    {
        this->serialNumber = jsonObj[param::serial_number::key].string_value();
        this->caseColour   = jsonObj[param::case_colour::key].string_value();
    }

    void PersonalizationData::setInvalidParamsAsDefault(const std::map<std::string, ParamModel> &paramsModel)
    {
        for (const auto &param : paramsModel) {
            if (param.second.mandatory == MandatoryParameter::no && !param.second.isValid) {
                LOG_ERROR("Optional parameter: %s  is invalid", param.first.c_str());
                if (param.first == param::serial_number::key) {
                    serialNumber = param.second.defaultValue;
                }
                else if (param.first == param::case_colour::key) {
                    caseColour = param.second.defaultValue;
                }
            }
        }
    }

} // namespace phone_personalization
