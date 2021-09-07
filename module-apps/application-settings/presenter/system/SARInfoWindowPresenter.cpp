// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SARInfoWindowPresenter.hpp"

SARInfoWindowPresenter::SARInfoWindowPresenter(std::unique_ptr<AbstractSARInfoRepository> &&sarInfoRepository)
    : sarInfoRepository{std::move(sarInfoRepository)}
{}

std::string SARInfoWindowPresenter::getSarInfo()
{
    return sarInfoRepository->getSarInfoText();
}
