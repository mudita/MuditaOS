// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <memory>
#include <Service/ServiceProxy.hpp>

namespace sys
{
    // mockup
    class Service
    {};

} // namespace sys

class Interface_imp : public service::ServiceProxy
{
  public:
    using ServiceProxy::getService;
    Interface_imp() = default;
    explicit Interface_imp(std::weak_ptr<sys::Service> s) : service::ServiceProxy(std::move(s))
    {}
};

TEST_CASE("Interface - not initialized")
{
    SECTION("base api")
    {
        auto interface = service::ServiceProxy();
        REQUIRE(!interface.isValid());
    }

    SECTION("inherited api")
    {
        auto interface = Interface_imp();
        REQUIRE_THROWS_AS(interface.getService(), std::runtime_error);
    }
}

TEST_CASE("Interface - initialized")
{
    auto service   = std::make_shared<sys::Service>();
    auto interface = Interface_imp(service);
    REQUIRE(interface.isValid());
    REQUIRE_NOTHROW(interface.getService());
}

TEST_CASE("Interface - expired")
{
    Interface_imp *interface;
    std::shared_ptr<sys::Service> expired;
    {
        auto service = std::make_shared<sys::Service>();
        interface    = new Interface_imp(service);
    }
    REQUIRE(!interface->isValid());
    REQUIRE_THROWS_AS(interface->getService(), std::runtime_error);
    delete interface;
}

TEST_CASE("Interface - copied")
{
    auto service    = std::make_shared<sys::Service>();
    auto interface  = Interface_imp(service);
    auto interface2 = interface;

    REQUIRE(interface.isValid());
    REQUIRE_NOTHROW(interface.getService());
    REQUIRE(interface2.isValid());
    REQUIRE_NOTHROW(interface2.getService());
    REQUIRE(interface2.getService() == interface.getService());
}
