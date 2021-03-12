// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-utils/log/log.hpp>
#include <module-utils/Utils.hpp>
#include <module-vfs/include/user/purefs/fs/filesystem.hpp>
#include <module-vfs/include/user/purefs/filesystem_paths.hpp>
#include "module-utils/json/json11.hpp"
#include <module-utils/re2/re2/re2.h>

namespace phone_personalization
{
    namespace param
    {
        namespace serial_number
        {
            constexpr inline auto key    = "serial";
            constexpr inline auto patern = "SN-[\\d]{3}-[\\d]{6}"; /// Need to get provided docs witch describes exact S/N format
            //constexpr inline auto length = 14;
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
                     [&]() { validateSerialNumber(param::serial_number::key); })},
            {param::case_colour::key,
             ParamModel(
                 param::case_colour::default_value,
                 param::case_colour::valid_values,
                 MandatoryParameter::no,
                 [&]() { validateByValues(param::case_colour::key);})}
        };

      protected:
        void validateByValues(const std::string &key);
        void validateSerialNumber(const std::string &key);

        json11::Json jsonObj = nullptr;

      public:
        static auto validateFileCRC(const std::filesystem::path &filePath) -> bool;
        auto validateJsonObject() -> bool;
        auto validateParameters() -> bool;


        explicit PersonalizationFileValidator(const json11::Json &jsonObj)
            : jsonObj{jsonObj}
        {}

        [[nodiscard]] auto getParamsValidationModel() const -> std::map<std::string, ParamModel>
        {
            return paramsModel;
        };
    };

    class PersonalizationData
    {
        std::map<std::string, std::string> parameters;

        void setParamsFromJson(const json11::Json &jsonObj);

      public:
        explicit PersonalizationData(const json11::Json &jsonObj)
        {
            setParamsFromJson(jsonObj);
        };

        void setInvalidParamsAsDefault(const std::map<std::string, ParamModel> &paramsModel);

        [[nodiscard]] auto getParameterByKey(const std::string &key) -> std::string
        {
            return parameters[key];
        };
    };

    class PersonalizationDataIntegrationProcess
    {
        std::unique_ptr<PersonalizationFileParser>    parser;
        std::unique_ptr<PersonalizationFileValidator> validator;
        std::unique_ptr<PersonalizationData> data;

    public:

        auto proceed(const std::filesystem::path &filePath) -> bool;

        [[nodiscard]] auto getData() -> std::unique_ptr<PersonalizationData>
        {
            return std::move(data);
        };

    };


} // namespace phone_personalization
