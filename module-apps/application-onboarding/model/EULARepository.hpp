// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "filesystem"

namespace app::onBoarding
{
    class AbstractEULARepository
    {
      public:
        virtual ~AbstractEULARepository() noexcept = default;

        virtual std::string getEulaText() = 0;
    };

    class EULARepository : public AbstractEULARepository
    {
      public:
        EULARepository(std::filesystem::path licensesPath, std::filesystem::path fileName);

        std::string getEulaText() override;

      private:
        const std::filesystem::path licensesPath;
        const std::filesystem::path fileName;
    };
} // namespace app::onBoarding
