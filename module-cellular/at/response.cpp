#include "response.hpp"
#include <Utils.hpp>

#include <algorithm>

namespace at
{
    namespace response
    {

        bool parseCSQ(std::string &response, std::string &result)
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
        bool parseCREG(std::string &response, uint32_t &result)
        {
            auto pos = response.find(',');
            if (pos != std::string::npos) {
                auto constexpr digitLength = 1;
                response                   = response.substr(pos + digitLength, digitLength);
                return utils::toNumeric(response, result);
            }
            return false;
        }
        bool parseCREG(std::string &response, std::string &result)
        {
            std::map<uint32_t, std::string> cregCodes;
            cregCodes.insert(std::pair<uint32_t, std::string>(0, "Not registred"));
            cregCodes.insert(std::pair<uint32_t, std::string>(1, "Registered, home network"));
            cregCodes.insert(std::pair<uint32_t, std::string>(2, "Not registered, searching"));
            cregCodes.insert(std::pair<uint32_t, std::string>(3, "Registration denied"));
            cregCodes.insert(std::pair<uint32_t, std::string>(4, "Unknown"));
            cregCodes.insert(std::pair<uint32_t, std::string>(5, "Registerd, roaming"));

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
            void parseNetworkFrequency(std::string &response)
            {
                auto tokens = utils::split(response, ",");
                for (auto el : tokens) {
                    LOG_INFO("token: %s", el.c_str());
                }
                auto constexpr qnwinfoResponseSize = 4;
                auto constexpr bandTokenPos        = 2;
                if (tokens.size() == qnwinfoResponseSize) {
                    auto constexpr gsmString   = "GSM";
                    auto constexpr wcdmaString = "WCDMA";
                    auto constexpr lteString   = "LTE";
                    if (tokens[bandTokenPos].find(gsmString) != std::string::npos ||
                        tokens[bandTokenPos].find(wcdmaString) != std::string::npos) {
                        LOG_INFO("Numeric band value.");
                    }
                    else if (tokens[bandTokenPos].find(lteString) != std::string::npos) {
                        LOG_INFO("String band value.");
                        parseLteBand(tokens[bandTokenPos]);
                    }
                }
            }
            uint32_t parseLteBand(std::string &band)
            {

                std::map<uint32_t, uint32_t> lteFreqs;
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_3, band_3_freq));

                auto constexpr toRemove    = "LTE BAND ";
                auto constexpr emptyString = "";
                utils::findAndReplaceAll(band, "\"", emptyString);
                utils::findAndReplaceAll(band, toRemove, emptyString);
                LOG_INFO("Band : %s", band.c_str());
                uint32_t bandValue;
                utils::toNumeric(band, bandValue);
                LOG_INFO("Band value: %lu", bandValue);
                auto freq = lteFreqs.find(bandValue);
                if (freq != lteFreqs.end()) {
                    LOG_INFO("Freq = %lu", freq->second);
                }

                return 1;
            }
        } // namespace qnwinfo
    }     // namespace response
} // namespace at
