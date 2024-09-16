// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <filesystem>

class AbstractSARInfoRepository
{
  public:
    virtual ~AbstractSARInfoRepository() noexcept = default;

    virtual std::string getSarInfoText() = 0;
};

class SARInfoRepository : public AbstractSARInfoRepository
{
  public:
    SARInfoRepository(std::filesystem::path certificationInfoPath, std::filesystem::path fileName);

    std::string getSarInfoText() override;

  private:
    const std::filesystem::path certificationInfoPath;
    const std::filesystem::path fileName;
};
