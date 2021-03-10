// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-utils/log/log.hpp>
#include <module-utils/Utils.hpp>
#include <module-vfs/include/user/purefs/fs/filesystem.hpp>
#include <module-vfs/include/user/purefs/filesystem_paths.hpp>
#include "module-utils/json/json11.hpp"

namespace phone_personalization
{
    namespace param
    {
        namespace serial_number
        {
            constexpr inline auto key    = "serial";
            constexpr inline auto prefix = "SN"; /// Need access to req witch describes SN format
            constexpr inline auto length = 14;
        } // namespace serial_number

        namespace case_colour
        {
            constexpr inline auto key                   = "case_colour";
            constexpr inline auto default_value         = "white";
            const std::vector<std::string> valid_values = {"white", "black"};
        } // namespace case_colour

    } // namespace param

    class PersonalizationFileParser
    {
        std::filesystem::path filePath;

      protected:
        auto loadFileContent() const -> std::string;

      public:
        explicit PersonalizationFileParser(const std::filesystem::path &filePath) : filePath{filePath} {};
        auto parseJson() const -> json11::Json;
    };

    enum class MandatoryParameter
    {
        no,
        yes
    };

    class ParamModel
    {
    public:
        std::string defaultValue;
        std::vector<std::string> validValues;
        MandatoryParameter mandatory;
        std::function<void()> validateCallback;
        bool isValid = false;

        ParamModel() = default;

        explicit ParamModel(const std::string &defaultValue,
                            const std::vector<std::string> &validValues,
                            MandatoryParameter mandatory,
                            const std::function<void()> &validateCallback)
                : defaultValue{defaultValue}, validValues{validValues}, mandatory{mandatory}, validateCallback{
                validateCallback} {};
    };

    class PersonalizationFileValidator
    {

        std::map<std::string, ParamModel> paramsModel = {
            {param::serial_number::key,
             ParamModel(
                     "",
                     {},
                     MandatoryParameter::yes,
                     [&]() { validateByFormat(param::serial_number::key); })},
            {param::case_colour::key,
             ParamModel(
                 param::case_colour::default_value,
                 param::case_colour::valid_values,
                 MandatoryParameter::no,
                 [&]() { validateByValues(param::case_colour::key);})}
        };

      protected:
        void validateByValues(const std::string &key);
        void validateByFormat(const std::string &key);

        json11::Json jsonObj = nullptr;
        std::filesystem::path filePath;

        auto validateFileCRC() -> bool;
        auto validateJsonObject() -> bool;
        auto validateParameters() -> bool;

      public:
        explicit PersonalizationFileValidator(const json11::Json &jsonObj, const std::filesystem::path &filePath)
            : jsonObj{jsonObj}, filePath{filePath}
        {}

        auto validate() -> bool;

        [[nodiscard]] auto getParamsModel() const -> std::map<std::string, ParamModel>
        {
            return paramsModel;
        };
    };

    class PersonalizationData
    {
        std::string serialNumber;
        std::string caseColour;

      public:
        explicit PersonalizationData() = default;

        void setParamsFromJson(const json11::Json &jsonObj);
        void setInvalidParamsAsDefault(const std::map<std::string, ParamModel> &paramsModel);

        [[nodiscard]] auto getSerialNumber() const -> std::string
        {
            return serialNumber;
        };

        [[nodiscard]] auto getCaseColour() const -> std::string
        {
            return caseColour;
        };
    };
} // namespace phone_personalization
