// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-utils/gsl/gsl>
#include "module-utils/bootconfig/src/bootconfig.cpp"
#include "PersonalizationFileValidation.hpp"

auto PersonalizationFileParser::loadFileAsString() -> std::string
{
    std::string content;
    std::ifstream in(filePath);
    if(!in.is_open())
    {
        LOG_FATAL("Error while loading file");
        return "";
    }
    std::getline(in, content, std::string::traits_type::to_char_type(std::string::traits_type::eof()));
    return content;
}

auto PersonalizationFileParser::parseJsonFromContent(const std::string &content) -> bool
{
    std::string parseErrors;
    std::string jsonContents = loadFileAsString();

    LOG_INFO("parsed %s: \"%s\"", filePath.c_str(), jsonContents.c_str());

    params = json11::Json::parse(jsonContents, parseErrors);

    if (parseErrors.length() != 0) {
        LOG_FATAL("JSON format error: %s", parseErrors.c_str());
        return false;
    }

    return true;
}

auto PersonalizationFileValidator::validateFileAndCRC() -> bool
{
    if (!boot::readAndVerifyCRC(filePath)) {
        LOG_ERROR("Invalid crc calculation!");
        return false;
    }

    LOG_INFO("CRC is correct");
    return true;
}

auto PersonalizationFileValidator::validateFileAndItsContent() -> bool
{
    auto content = PersonalizationFileParser::loadFileAsString();
    if(content.empty()){
        return false;
    }

    return PersonalizationFileParser::parseJsonFromContent(content);
}

auto PersonalizationFileValidator::validateParameters(const std::map<std::string, std::vector<std::string>> &parametersMap) -> bool
{
    std::map<std::string, std::string> optionalParams; ///Maybe even not need to store this map (defalt values can be replaced if param is valid)
    std::map<std::string, std::string> defaultValues; ///TODO: move to consts also parametersMap (should be optional parameters map -> serial number should checked separetly)

    for (auto it = parametersMap.begin(); it != parametersMap.end(); it++)
    {
        auto result = std::find(std::begin(it->second),std::end(it->second), params[it->first].string_value());
        if(result == std::end(it->second)){
            LOG_ERROR("Optional parameter invalid!");
            ///TODO: maybe create a map as default output params
            optionalParams[it->first] = defaultValues[it->first];
        }
    }
}
