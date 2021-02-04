// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-utils/gsl/gsl_util>
#include "module-utils/i18n/i18n.hpp"
#include "module-utils/Utils.hpp"

#include "EULARepository.hpp"

namespace app::onBoarding
{
    EULARepository::EULARepository(std::filesystem::path licensesPath, std::filesystem::path fileName)
        : licensesPath(std::move(licensesPath)), fileName(std::move(fileName))
    {}

    std::string EULARepository::getEulaText()
    {
        auto displayLanguageName = utils::localize.getDisplayLanguage();
        auto eulaFile            = utils::filesystem::openFile(licensesPath / displayLanguageName / fileName);
        auto fileHandlerCleanup  = gsl::finally([&eulaFile]() { utils::filesystem::closeFile(eulaFile); });

        if (eulaFile == nullptr) {
            eulaFile = utils::filesystem::openFile(licensesPath / utils::i18n::DefaultLanguage / fileName);

            if (eulaFile == nullptr) {
                throw std::runtime_error("EULA assets missing in system!");
            }
        }
        auto eulaText = utils::filesystem::readFile(eulaFile);

        return eulaText;
    }
} // namespace app::onBoarding
