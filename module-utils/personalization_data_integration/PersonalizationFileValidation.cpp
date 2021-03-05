// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-utils/gsl/gsl>
#include "module-utils/bootconfig/src/bootconfig.cpp"
#include "PersonalizationFileValidation.hpp"


namespace phone_personalization {


    auto PersonalizationFileParser::loadFileContent() -> std::string {
        std::string content;
        std::ifstream in(filePath);
        if (!in.is_open()) {
            LOG_FATAL("Error while loading personalization file");
            return "";
        }
        std::getline(in, content, std::string::traits_type::to_char_type(std::string::traits_type::eof()));
        return content;
    }

    auto PersonalizationFileParser::parseJsonFromContent(const std::string &content) -> bool {

        std::string parseErrors;
        LOG_INFO("parsed %s: \"%s\"", filePath.c_str(), content.c_str());

        jsonObj = json11::Json::parse(content, parseErrors);

        if (parseErrors.length() != 0) {
            LOG_FATAL("JSON format error: %s", parseErrors.c_str());
            return false;
        }

        return true;
    }

    auto PersonalizationFileValidator::validateFileAndCRC() -> bool {
        if (!boot::readAndVerifyCRC(filePath)) {
            LOG_ERROR("Invalid crc calculation!");
            return false;
        }

        LOG_INFO("CRC is correct");
        return true;
    }

    auto PersonalizationFileValidator::validateFileAndItsContent() -> bool {
        auto content = PersonalizationFileParser::loadFileContent();
        if (content.empty()) {
            return false;
        }

        return PersonalizationFileParser::parseJsonFromContent(content);
    }

    auto PersonalizationFileValidator::validateSerialNumber() -> bool
    {
        if(jsonObj != nullptr)
        {
            auto serialNumber = jsonObj[param::serial_number::key].string_value();
            if(serialNumber.empty())
            {
                LOG_ERROR("Serial number: '%s' is epmty", serialNumber.c_str());
                return false;
            }
            if(serialNumber.find(param::serial_number::prefix) != 0)
            {
                LOG_ERROR("Wrong format of serial number: '%s'", serialNumber.c_str());
                return false;
            }
            if(serialNumber.size() != param::serial_number::length)
            {
                LOG_ERROR("Serial number: '%s' has wrong size", serialNumber.c_str());
                return false;
            }
            LOG_INFO("Valid serial number: %s", serialNumber.c_str());
            return true;
        }

        LOG_ERROR("Empty json object!");
        return false;
    }

    void PersonalizationFileValidator::validateOptionalParams()
    {
        if(jsonObj != nullptr)
        {
            for (auto param = optionalParams.begin(); param != optionalParams.end(); param++)
            {
                auto result = std::find(std::begin(param->second.validValues), std::end(param->second.validValues),
                                        jsonObj[param->first].string_value());

                if (result == std::end(param->second.validValues))
                {
                    LOG_ERROR("Optional parameter: '%s' is invalid! Parameter value remains set to default",
                              param->first.c_str());
                    param->second.isValid = false;
                }
                else
                {
                    LOG_INFO("Optional parameter: '%s' is valid", param->first.c_str());
                    param->second.isValid = true;
                }
            }
            LOG_INFO("Optional parameters are valid");
        }
        LOG_ERROR("Params json object not parsed!");
    }

    auto PersonalizationFileValidator::validate() -> bool
    {
        if(!validateFileAndCRC())
        {
            return false;
        }
        if(!validateFileAndItsContent())
        {
            return false;
        }
        if(!validateSerialNumber())
        {
            return false;
        }
        validateOptionalParams();

        return true;
    }

    auto PersonalizationParamsGetter::getParams() -> std::map<std::string, std::string>
    {
        auto jsonObj = personalizationParser.getJsonObject();
        auto optionalParams = personalizationParser.getDefaultOptionalParamsMap();

        if(jsonObj != nullptr)
        {
            parameters[param::serial_number::key] = jsonObj[param::serial_number::key].string_value();
            for (auto param : optionalParams) {
                if (param.second.isValid) {
                    parameters[param.first] = jsonObj[param.first].string_value();
                } else {
                    parameters[param.first] = param.second.defaultValue;
                }
            }
            return parameters;
        }
        LOG_ERROR("Parameter object not parsed");
        return parameters;
    }

} // namespace phone_personalization
