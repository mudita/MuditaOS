
#pragma once

#include "Result.hpp"
#include <log/log.hpp>
#include <map>
#include <memory>
#include <string>

namespace at
{
    namespace response
    {
        std::vector<std::string> tokenize(std::string &response, std::string separator = ",");
        bool parseCSQ(std::string &response, std::string &result);
        bool parseCSQ(std::string &cellularResponse, uint32_t &result);
        bool parseCREG(std::string &response, uint32_t &result);
        bool parseCREG(std::string &response, std::string &result);
        bool parseQNWINFO(std::string &response, std::string &result);

        namespace creg
        {
            bool isRegistered(uint32_t commandData);
        }
        namespace qnwinfo
        {

            auto constexpr gsmString   = "GSM";
            auto constexpr wcdmaString = "WCDMA";

            auto constexpr band_1  = 1;
            auto constexpr band_2  = 2;
            auto constexpr band_3  = 3;
            auto constexpr band_4  = 4;
            auto constexpr band_5  = 5;
            auto constexpr band_7  = 7;
            auto constexpr band_8  = 8;
            auto constexpr band_12 = 12;
            auto constexpr band_13 = 13;
            auto constexpr band_18 = 18;
            auto constexpr band_19 = 19;
            auto constexpr band_20 = 20;
            auto constexpr band_25 = 25;
            auto constexpr band_26 = 26;
            auto constexpr band_28 = 28;
            auto constexpr band_38 = 38;
            auto constexpr band_40 = 40;
            auto constexpr band_41 = 41;

            auto constexpr band_3_freq = 1800;
            auto constexpr band_1_freq = 2100;
            auto constexpr band_2_freq = 1900;

            auto constexpr band_4_freq  = 1700;
            auto constexpr band_5_freq  = 850;
            auto constexpr band_7_freq  = 2600;
            auto constexpr band_8_freq  = 900;
            auto constexpr band_12_freq = 700;
            auto constexpr band_13_freq = 700;
            auto constexpr band_18_freq = 850;
            auto constexpr band_19_freq = 850;
            auto constexpr band_20_freq = 800;
            auto constexpr band_25_freq = 1900;
            auto constexpr band_26_freq = 850;
            auto constexpr band_28_freq = 700;
            auto constexpr band_38_freq = 2600;
            auto constexpr band_40_freq = 2300;
            auto constexpr band_41_freq = 2500;

            uint32_t parseNetworkFrequency(std::string &response);
            uint32_t parseNumericBandString(std::string &string);
            uint32_t parseLteBandString(std::string &string);
        } // namespace qnwinfo
    }     // namespace response
} // namespace at
