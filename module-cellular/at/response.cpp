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

            auto pos = response.find(toErase);
            if (pos != std::string::npos) {
                response.erase(pos, toErase.length());
                result = response;
                return true;
            }
            return false;
        }
        bool parseCSQ(std::string &cellularResponse, uint32_t &result)
        {

            std::string CSQstring;
            if (parseCSQ(cellularResponse, CSQstring)) {
                auto pos = CSQstring.find(',');
                if (pos != std::string::npos) {
                    CSQstring = CSQstring.substr(0, pos);
                    return utils::toNumeric(CSQstring, result);
                }
            }
            return false;
        }
        namespace creg
        {
            bool isRegistered(uint32_t commandData)
            {

                // CREG command returns 1 when registered in home network, 5 when registered in roaming
                constexpr uint32_t registeredHome    = 1;
                constexpr uint32_t registeredRoaming = 5;

                if (commandData == registeredHome || commandData == registeredRoaming) {
                    return true;
                }
                return false;
            }
        } // namespace creg
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
            cregCodes.insert(std::pair<uint32_t, std::string>(0, "Not registered"));
            cregCodes.insert(std::pair<uint32_t, std::string>(1, "Registered, home network"));
            cregCodes.insert(std::pair<uint32_t, std::string>(2, "Not registered, searching"));
            cregCodes.insert(std::pair<uint32_t, std::string>(3, "Registration denied"));
            cregCodes.insert(std::pair<uint32_t, std::string>(4, "Unknown"));
            cregCodes.insert(std::pair<uint32_t, std::string>(5, "Registered, roaming"));

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
            uint32_t parseNetworkFrequency(std::string &response)
            {
                auto tokens = utils::split(response, ",");

                auto constexpr qnwinfoResponseSize = 4;
                auto constexpr bandTokenPos        = 2;
                if (tokens.size() == qnwinfoResponseSize) {

                    auto constexpr lteString = "LTE";
                    if (tokens[bandTokenPos].find(gsmString) != std::string::npos ||
                        tokens[bandTokenPos].find(wcdmaString) != std::string::npos) {
                        return parseNumericBandString(tokens[bandTokenPos]);
                    }
                    else if (tokens[bandTokenPos].find(lteString) != std::string::npos) {

                        return parseLteBandString(tokens[bandTokenPos]);
                    }
                }
                return 0;
            }
            uint32_t parseNumericBandString(std::string &string)
            {
                utils::findAndReplaceAll(string, gsmString, "");
                utils::findAndReplaceAll(string, wcdmaString, "");
                utils::findAndReplaceAll(string, " ", "");
                utils::findAndReplaceAll(string, "\"", "");

                uint32_t freq = 0;
                utils::toNumeric(string, freq);
                return freq;
            }
            uint32_t parseLteBandString(std::string &string)
            {

                std::map<uint32_t, uint32_t> lteFreqs;
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_1, band_1_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_2, band_2_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_3, band_3_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_4, band_4_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_5, band_5_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_7, band_7_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_8, band_8_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_12, band_12_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_13, band_13_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_18, band_18_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_20, band_20_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_25, band_25_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_26, band_26_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_28, band_28_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_38, band_38_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_40, band_40_freq));
                lteFreqs.insert(std::pair<uint32_t, uint32_t>(band_41, band_41_freq));

                auto constexpr toRemove    = "LTE BAND ";
                auto constexpr emptyString = "";
                utils::findAndReplaceAll(string, "\"", emptyString);
                utils::findAndReplaceAll(string, toRemove, emptyString);

                uint32_t band;
                utils::toNumeric(string, band);

                auto freq = lteFreqs.find(band);
                if (freq != lteFreqs.end()) {
                    return freq->second;
                }

                return 0;
            }
        } // namespace qnwinfo
    }     // namespace response
} // namespace at
