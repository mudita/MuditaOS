// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SARInfoRepository.hpp"

#include <gsl_util>
#include <i18n/i18n.hpp>

#include <fstream>

SARInfoRepository::SARInfoRepository(std::filesystem::path certificationInfoPath, std::filesystem::path fileName)
    : certificationInfoPath(std::move(certificationInfoPath)), fileName(std::move(fileName))
{}

std::string SARInfoRepository::getSarInfoText()
{
    auto displayLanguageName = utils::localize.getDisplayLanguage();
    auto sarInfoFile         = std::ifstream(certificationInfoPath / displayLanguageName / fileName);

    if (!sarInfoFile.is_open()) {
        sarInfoFile.open(certificationInfoPath / utils::i18n::DefaultLanguage / fileName);

        if (!sarInfoFile.is_open()) {
            throw std::runtime_error("SAR info assets are missing in the system!");
        }
    }
    std::ostringstream sarInfoText;
    sarInfoText << sarInfoFile.rdbuf();

    return sarInfoText.str();
}
