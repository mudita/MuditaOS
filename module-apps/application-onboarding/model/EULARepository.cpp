// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EULARepository.hpp"
#include <gsl/util>
#include <i18n/i18n.hpp>
#include <fstream>
#include <sstream>

namespace app::onBoarding
{
    EULARepository::EULARepository(std::filesystem::path licensesPath, std::filesystem::path fileName)
        : licensesPath(std::move(licensesPath)), fileName(std::move(fileName))
    {}

    std::string EULARepository::getEulaText()
    {
        const auto &displayLanguageName = utils::getDisplayLanguage();
        auto eulaFile                   = std::ifstream(licensesPath / displayLanguageName / fileName);
        auto fileHandlerCleanup         = gsl::finally([&eulaFile]() { eulaFile.close(); });

        if (!eulaFile.is_open()) {
            eulaFile.open(licensesPath / utils::getDefaultLanguage() / fileName);

            if (!eulaFile.is_open()) {
                throw std::runtime_error("EULA assets missing in system!");
            }
        }
        std::ostringstream eulaText;
        eulaText << eulaFile.rdbuf();

        return eulaText.str();
    }
} // namespace app::onBoarding
