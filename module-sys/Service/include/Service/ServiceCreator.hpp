// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ServiceManifest.hpp"

#include <memory>
#include <tuple>

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

    template <typename T, typename... Args>
    class ServiceCreator : public BaseServiceCreator
    {
      public:
        using BaseServiceCreator::BaseServiceCreator;

        explicit ServiceCreator(ServiceManifest &&manifest, Args &&...args)
            : BaseServiceCreator(std::move(manifest)), savedArgs(std::make_tuple<Args...>(std::forward<Args>(args)...))
        {}

        [[nodiscard]] auto create() const -> std::shared_ptr<Service> override
        {
            return std::apply([](auto... args) { return std::make_shared<T>(std::forward<Args>(args)...); }, savedArgs);
        }

      private:
        std::tuple<Args...> savedArgs;
    };

    /**
     * Creates a ServiceCreator object for Service T.
     * @tparam T        Type of service that is to be created
     * @tparam Args     Types of parameter pack arguments
     * @param args      Parameter pack that is to be forwarded to the service constructor.
     * @return ServiceCreator for Service T.
     */
    template <typename T, typename... Args>
    std::unique_ptr<BaseServiceCreator> CreatorFor(Args &&...args) noexcept
    {
        if constexpr (sizeof...(args)) {
            static_assert(not std::is_reference_v<Args...>,
                          "Do not pass const T& or T& as parameters. Use r-values (std::move) instead");
        }
        return std::make_unique<ServiceCreator<T, Args...>>(ManifestOf<T>(), std::forward<Args>(args)...);
    }
} // namespace sys
