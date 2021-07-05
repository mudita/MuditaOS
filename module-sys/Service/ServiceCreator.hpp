// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ServiceManifest.hpp"

#include <memory>

namespace sys
{
    class Service;

    class BaseServiceCreator
    {
      public:
        explicit BaseServiceCreator(ServiceManifest &&manifest) noexcept : manifest{std::move(manifest)}
        {}
        virtual ~BaseServiceCreator() noexcept = default;

        [[nodiscard]] virtual std::shared_ptr<Service> create() const = 0;

        [[nodiscard]] auto getName() const noexcept -> const ServiceManifest::ServiceName &
        {
            return manifest.name;
        }

        [[nodiscard]] auto getDependencies() const noexcept -> const std::vector<ServiceManifest::ServiceName> &
        {
            return manifest.dependencies;
        }

        [[nodiscard]] auto getStartTimeout() const noexcept -> ServiceManifest::Timeout
        {
            return manifest.timeout;
        }

      private:
        ServiceManifest manifest;
    };

    template <typename T> class ServiceCreator : public BaseServiceCreator
    {
      public:
        using BaseServiceCreator::BaseServiceCreator;

        [[nodiscard]] auto create() const -> std::shared_ptr<Service> override
        {
            return std::make_shared<T>();
        }
    };

    template <typename T> std::unique_ptr<BaseServiceCreator> CreatorFor() noexcept
    {
        return std::make_unique<ServiceCreator<T>>(ManifestOf<T>());
    }
} // namespace sys
