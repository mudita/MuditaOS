// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Anonymize.hpp"

namespace utils::anonymize
{
    // Anonymized text will replace by this character
    constexpr auto anonymizationCharacter = '*';

    // Amount of 'anonymizationCharacter' used during anonymization can be fixed,
    // otherwise their amount depend on length of substring to anonymize
    constexpr auto isAmountOfAnonymizationCharactersFixed = false;
    constexpr auto fixedAmountOfAnonymizationCharacters   = 4;

    // Substrings to find logs for anonymization from the cellular logs
    constexpr auto cellularSubstringCPIN = "CPIN";
    constexpr auto cellularSubstringCLCC = "CLCC";
    constexpr auto cellularSubstringATD  = "ATD";
    constexpr auto cellularSubstringCPBR = "CPBR";
    constexpr auto cellularSubstringCSPN = "CSPN";

    std::string anonymizeInQuotationMarks(const std::string &textToAnonymize, std::size_t singsToLeaveAtEnd)
    {
        constexpr auto quotesMark = '\"';
        std::size_t startPosition = 0;
        std::size_t endPosition   = 0;
        auto anonymizedText       = textToAnonymize;

        // Find the position of the substring within the input string
        while (startPosition < anonymizedText.length()) {
            startPosition = anonymizedText.find(quotesMark, startPosition);
            endPosition   = anonymizedText.find(quotesMark, startPosition + 1);

            if (startPosition != std::string::npos && endPosition != std::string::npos) {
                // Extract the substring between the quotes
                const auto &extractedSubstring =
                    anonymizedText.substr(startPosition + 1, endPosition - startPosition - 1);

                // Determine the length of the extracted substring
                const auto extractedLength = extractedSubstring.length();

                // Amount of signs to left can't be higher than extracted substring
                singsToLeaveAtEnd = std::min(extractedLength, singsToLeaveAtEnd);

                // Determine amount of anonymization characters
                const auto amountOfAnonymizationCharacters = isAmountOfAnonymizationCharactersFixed
                                                                 ? fixedAmountOfAnonymizationCharacters
                                                                 : extractedLength - singsToLeaveAtEnd;

                // Replace the substring with the appropriate number of '*' characters
                anonymizedText.replace(startPosition + 1,
                                       extractedLength - singsToLeaveAtEnd,
                                       amountOfAnonymizationCharacters,
                                       anonymizationCharacter);
                startPosition = endPosition + 1;
            }
            else {
                break;
            }
        }
        return anonymizedText;
    }

    std::string anonymizeNumbers(const std::string &textToAnonymize, std::size_t singsToLeaveAtEnd)
    {
        std::size_t startPosition = 0;
        std::size_t endPosition   = 0;
        auto isDigitFunction      = [](char c) { return std::isdigit(c); };
        auto anonymizedText       = textToAnonymize;
        auto startSubstring       = anonymizedText.begin();
        auto endSubstring         = anonymizedText.begin();

        while (startPosition < anonymizedText.length()) {
            // Find first number in string
            startSubstring = std::find_if(endSubstring, anonymizedText.end(), isDigitFunction);

            // If first number found then find last number and anonymize whole number in string
            if (startSubstring != anonymizedText.end()) {
                endSubstring = std::find_if_not(startSubstring, anonymizedText.end(), isDigitFunction);

                // Calculate positions
                startPosition = std::distance(anonymizedText.begin(), startSubstring);
                endPosition   = std::distance(anonymizedText.begin(), endSubstring);

                if (startPosition != std::string::npos && endPosition != std::string::npos) {
                    // Extract the substring contained only numbers
                    const auto &extractedSubstring =
                        anonymizedText.substr(startPosition + 1, endPosition - startPosition);

                    // Determine the length of the extracted substring
                    const auto extractedLength = extractedSubstring.length();

                    // Amount of signs to left can't be higher than extracted substring
                    singsToLeaveAtEnd = std::min(extractedLength, singsToLeaveAtEnd);

                    // Determine amount of anonymization characters
                    const auto amountOfAnonymizationCharacters = isAmountOfAnonymizationCharactersFixed
                                                                     ? fixedAmountOfAnonymizationCharacters
                                                                     : extractedLength - singsToLeaveAtEnd;

                    // Replace the substring with the appropriate number of '*' characters
                    anonymizedText.replace(startPosition,
                                           extractedLength - singsToLeaveAtEnd,
                                           amountOfAnonymizationCharacters,
                                           anonymizationCharacter);

                    startPosition = endPosition + 1;
                }
                else {
                    break;
                }
            }
            else {
                break;
            }
        }
        return anonymizedText;
    }

    std::string anonymizeCellularIfNecessary(const std::string &text)
    {
        std::map<std::string, std::function<std::string()>> anonymizeOption{
            {cellularSubstringCPIN,
             [&text]() { return anonymizeInQuotationMarks(text, SIGNS_TO_LEAVE_FOR_PIN_AND_PUK); }},
            {cellularSubstringCLCC,
             [&text]() { return anonymizeInQuotationMarks(text, SIGNS_TO_LEAVE_FOR_PHONE_NUMBERS); }},
            {cellularSubstringATD, [&text]() { return anonymizeNumbers(text, SIGNS_TO_LEAVE_FOR_PHONE_NUMBERS); }},
            {cellularSubstringCPBR,
             [&text]() { return anonymizeInQuotationMarks(text, SIGNS_TO_LEAVE_FOR_PHONE_NUMBERS); }},
            {cellularSubstringCSPN,
             [&text]() { return anonymizeInQuotationMarks(text, SIGNS_TO_LEAVE_FOR_NET_PROVIDER_NAME); }}};

        for (const auto &[key, anonymizeFunction] : anonymizeOption) {
            if (text.find(key) != std::string::npos) {
                return anonymizeFunction();
            }
        }
        return text;
    }
} // namespace utils::anonymize
