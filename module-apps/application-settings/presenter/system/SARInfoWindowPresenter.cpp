// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SARInfoWindowPresenter.hpp"
#include <log/log.hpp>

SARInfoWindowPresenter::SARInfoWindowPresenter(std::unique_ptr<AbstractSARInfoRepository> &&sarInfoRepository)
    : sarInfoRepository{std::move(sarInfoRepository)}
{}

std::string SARInfoWindowPresenter::getSarInfo()
{
    try {
        const auto sarInfoText = sarInfoRepository->getSarInfoText();
        return sarInfoText;
    }
    catch (const std::runtime_error &e) {
        LOG_ERROR("Failed to get SAR info text! Error: %s", e.what());
        return "";
    }
}
