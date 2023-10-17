// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ProfileConfigUtils.hpp"
#include <fstream>
#include <log/log.hpp>
#include <json11.hpp>
#include <magic_enum.hpp>
namespace audio
{
    namespace strings
    {
        constexpr inline auto samplerate        = "samplerate";
        constexpr inline auto bitWidth          = "bitWidth";
        constexpr inline auto flags             = "flags";
        constexpr inline auto outputVolume      = "outputVolume";
        constexpr inline auto outputPath        = "outputPath";
        constexpr inline auto inputGain         = "inputGain";
        constexpr inline auto inputPath         = "inputPath";
        constexpr inline auto filterParams      = "filterParams";
        constexpr inline auto filterType        = "filterType";
        constexpr inline auto frequency         = "frequency";
        constexpr inline auto Q                 = "Q";
        constexpr inline auto gain              = "gain";
        constexpr inline auto playbackPathGain  = "playbackPathGain";
        constexpr inline auto playbackPathAtten = "playbackPathAtten";
    } // namespace strings

    namespace utils
    {

        template <typename E, typename T>
        constexpr inline typename std::enable_if<std::is_enum<E>::value && std::is_integral<T>::value, E>::type toEnum(
            T value) noexcept
        {
            return static_cast<E>(value);
        }

        equalizer::FilterType toFilterType(const std::string &filterName)
        {
            auto filterType = magic_enum::enum_cast<equalizer::FilterType>(filterName);
            if (filterType.has_value()) {
                return filterType.value();
            }
            else {
                LOG_ERROR("Unknown filter type, using none");
                return equalizer::FilterType::None;
            }
        }

        const std::string readFileToString(std::filesystem::path filePath)
        {
            std::ifstream file;
            std::string configString;
            LOG_DEBUG("Reading %s ...", filePath.c_str());
            file.open(filePath);
            if (not file.is_open()) {
                LOG_ERROR("Can't open profile configuration file, using defaults!");
                throw std::invalid_argument("Can't open file!");
            }
            while (file) {
                std::string line;
                std::getline(file, line);
                configString += line;
            }
            file.close();
            return configString;
        }
    } // namespace utils

    const audio::codec::Configuration loadConfigurationFromFile(std::filesystem::path filePath)
    {

        auto configString = utils::readFileToString(filePath);
        audio::codec::Configuration config;

        json11::Json configJson;
        std::string err;
        configJson = json11::Json::parse(configString.c_str(), err);
        if (!err.empty()) {
            LOG_ERROR("Failed parsing device string!");
            throw std::invalid_argument("Can't parse the file!");
        }
        config.sampleRate_Hz = configJson[strings::samplerate].int_value();
        config.bitWidth      = configJson[strings::bitWidth].int_value();
        config.flags         = configJson[strings::flags].int_value();
        config.outputVolume  = configJson[strings::outputVolume].number_value();
        config.inputGain     = configJson[strings::inputGain].number_value();
        config.inputPath     = utils::toEnum<codec::InputPath>(configJson[strings::inputPath].int_value());
        config.outputPath    = utils::toEnum<codec::OutputPath>(configJson[strings::outputPath].int_value());

        std::uint8_t playbackPathGain  = configJson[strings::playbackPathGain].int_value();
        std::uint8_t playbackPathAtten = configJson[strings::playbackPathAtten].int_value();

        constexpr std::uint8_t playbackPathGainMax = 3;
        if (playbackPathGain <= playbackPathGainMax) {
            config.playbackPathGain = playbackPathGain;
        }
        else {
            LOG_WARN("playbackPathGain value out of range (%u), using fallback value %u!",
                     playbackPathGain,
                     config.playbackPathGain);
        }

        constexpr std::uint8_t playbackPathAttenMax = 15;
        if (playbackPathAtten <= playbackPathAttenMax) {
            config.playbackPathAtten = playbackPathAtten;
        }
        else {
            LOG_WARN("playbackPathAtten value out of range (%u), using fallback value %u!",
                     playbackPathAtten,
                     config.playbackPathAtten);
        }

        json11::Json::array paramsArray;
        audio::equalizer::Equalizer filterParams;
        paramsArray = configJson[strings::filterParams].array_items();

        for (size_t i = 0; i < equalizer::bands; i++) {
            auto filterType = utils::toFilterType(paramsArray[i][strings::filterType].string_value());
            auto frequency  = paramsArray[i][strings::frequency].number_value();
            auto samplerate = paramsArray[i][strings::samplerate].int_value();
            auto Q          = paramsArray[i][strings::Q].number_value();
            auto gain       = paramsArray[i][strings::gain].number_value();

            filterParams.at(i) = qfilter_CalculateCoeffs(filterType, frequency, samplerate, Q, gain);
        }

        config.filterCoefficients = filterParams;
        return config;
    }

} // namespace audio
