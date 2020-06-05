
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
        bool parseCREG(std::string &response, uint32_t &result);
        bool parseCREG(std::string &response, std::string &result);
        bool parseQNWINFO(std::string &response, std::string &result);

        namespace qnwinfo
        {

            auto constexpr band_3 = 3;

            auto constexpr band_3_freq = 1800;

            void parseNetworkFrequency(std::string &response);
            uint32_t parseLteBand(std::string &band);
        } // namespace qnwinfo
    }     // namespace response
} // namespace at
