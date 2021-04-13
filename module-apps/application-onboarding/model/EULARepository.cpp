// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-utils/gsl/gsl_util>
#include "module-utils/i18n/i18n.hpp"

#include "EULARepository.hpp"
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
        auto eulaFile            = std::ifstream(licensesPath / displayLanguageName / fileName);
        auto fileHandlerCleanup  = gsl::finally([&eulaFile]() { eulaFile.close(); });

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
