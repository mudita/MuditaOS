// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-utils/gsl/gsl>
#include "module-utils/bootconfig/src/bootconfig.cpp"
#include "PersonalizationFileValidation.hpp"

namespace phone_personalization
{

    auto PersonalizationFileParser::loadFileContent() const -> std::string
    {
        std::ifstream in(filePath);
        if (!in.is_open()) {
            LOG_FATAL("Error while loading personalization file");
            return "";
        }
        std::string content( (std::istreambuf_iterator<char>(in) ),
                             (std::istreambuf_iterator<char>()) );
        return content;
    }

    auto PersonalizationFileParser::parseJson() const -> json11::Json
    {

        auto content = loadFileContent();

        std::string parseErrors;
        LOG_INFO("parsed %s: \"%s\"", filePath.c_str(), content.c_str());

        auto jsonObj = json11::Json::parse(content, parseErrors);

        if (!parseErrors.empty()) {
            LOG_FATAL("JSON format error: %s", parseErrors.c_str());
            return nullptr;
        }

        return jsonObj;
    }

    void PersonalizationFileValidator::validateSerialNumber(const std::string &key)
    {
        auto regex = RE2(param::serial_number::patern);
        paramsModel[key].isValid = re2::RE2::FullMatch(jsonObj[key].string_value(), regex);
        paramsModel[key].isValid ? LOG_FATAL("Serial number: %s is invalid", jsonObj[key].string_value().c_str()) : LOG_INFO("Serial number is valid");
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

    auto PersonalizationFileValidator::validateFileCRC(const std::filesystem::path &filePath) -> bool
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

    void PersonalizationData::setParamsFromJson(const json11::Json &jsonObj)
    {
        parameters[param::serial_number::key] = jsonObj[param::serial_number::key].string_value();
        parameters[param::case_colour::key] = jsonObj[param::case_colour::key].string_value();
    }

    void PersonalizationData::setInvalidParamsAsDefault(const std::map<std::string, ParamModel> &paramsModel)
    {
        for (const auto &param : paramsModel) {
            if (param.second.mandatory == MandatoryParameter::no && !param.second.isValid) {
                LOG_ERROR("Optional parameter: %s  is invalid", param.first.c_str());
                parameters[param.first] = param.second.defaultValue;
            }
        }
    }

    auto PersonalizationDataIntegrationProcess::proceed(const std::filesystem::path &filePath) -> bool
    {
        if (!PersonalizationFileValidator::validateFileCRC(filePath)) {
            return false;
        }

        parser = std::make_unique<PersonalizationFileParser>(filePath);
        auto jsonObj = parser->parseJson();

        validator = std::make_unique<PersonalizationFileValidator>(jsonObj);

        if (!validator->validateJsonObject()) {
            return false;
        }
        if (!validator->validateParameters()) {
            return false;
        }

        data = std::make_unique<PersonalizationData>(jsonObj);
        data->setInvalidParamsAsDefault(validator->getParamsValidationModel());

        return true;
    }


} // namespace phone_personalization
