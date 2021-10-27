// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "country.hpp"
#include "PhoneNumber.hpp"
#include "NumberHolderMatcher.hpp"

#include <initializer_list>
#include <string>
#include <vector>

using namespace utils;

class DummyHolder
{
    PhoneNumber number;

  public:
    DummyHolder(const PhoneNumber &number) : number(number)
    {}
    const PhoneNumber &getNumber() const
    {
        return number;
    }
};

static constexpr auto DefaultPageSize = 1; // so the paging mechanism is checked.

const static std::string NumberAPlNational = "600123456";
const static std::string NumberAPlE164     = "+48600123456";
const static PhoneNumber NumberAPlValid(NumberAPlE164, country::Id::UNKNOWN);
const static PhoneNumber NumberAPlInvalid(NumberAPlNational, country::Id::UNKNOWN);
const static PhoneNumber NumberAPlValidNational(NumberAPlNational, country::Id::POLAND);

const static std::string NumberBPlNational = "500123456";
const static std::string NumberBPlE164     = "+48500123456";
const static PhoneNumber NumberBPlValid(NumberBPlE164, country::Id::UNKNOWN);
const static PhoneNumber NumberBPlInvalid(NumberBPlNational, country::Id::UNKNOWN);
const static PhoneNumber NumberBPlValidNational(NumberBPlNational, country::Id::POLAND);

const static std::string NumberCPlNational = "500555555";
const static std::string NumberCPlE164     = "+48500555555";
const static PhoneNumber NumberCPlValid(NumberCPlE164, country::Id::UNKNOWN);
const static PhoneNumber NumberCPlInvalid(NumberCPlNational, country::Id::UNKNOWN);
const static PhoneNumber NumberCPlValidNational(NumberCPlNational, country::Id::POLAND);

const static PhoneNumber invalidNumber("456", country::Id::UNKNOWN);

std::initializer_list<PhoneNumber> all_test_numbers = {NumberAPlValid,
                                                       NumberAPlInvalid,
                                                       NumberAPlValidNational,
                                                       NumberBPlValid,
                                                       NumberBPlInvalid,
                                                       NumberBPlValidNational,
                                                       invalidNumber};

auto make_test_vector(std::initializer_list<PhoneNumber> lnumber)
{
    std::vector<DummyHolder> v;
    for (auto &number : lnumber) {
        v.push_back(DummyHolder(number));
    }

    return v;
}

auto retrieve_numbers(const std::vector<DummyHolder> &holders, unsigned int offset, unsigned int limit)
{
    const auto size = holders.size();
    if (offset > size - 1) {
        return std::vector<DummyHolder>{};
    }
    auto lastIndex = std::clamp<unsigned int>(offset + limit, offset, size);
    return std::vector<DummyHolder>(holders.begin() + offset, holders.begin() + lastIndex);
}

TEST_CASE("Number matcher - basics")
{
    std::vector<DummyHolder> numberHolders = {DummyHolder(PhoneNumber("500123456")),
                                              DummyHolder(PhoneNumber("600123456"))};
    auto provider = [&numberHolders]([[maybe_unused]] const utils::PhoneNumber &number, auto offset, auto limit) {
        return retrieve_numbers(numberHolders, offset, limit);
    };
    NumberHolderMatcher<std::vector, DummyHolder> matcher{std::move(provider), DefaultPageSize};

    auto match = matcher.bestMatch(PhoneNumber("600123456"));
    REQUIRE(match.has_value());
    REQUIRE(match->getNumber().get() == "600123456");

    // match again to see if search is started at the beginning again
    auto newMatch = matcher.bestMatch(PhoneNumber("600123456"));
    REQUIRE(newMatch.has_value());
    REQUIRE(newMatch->getNumber().get() == "600123456");
}

TEST_CASE("Number matcher - match incoming (full list)")
{
    auto dummyHolders = make_test_vector(all_test_numbers);
    auto provider     = [&dummyHolders]([[maybe_unused]] const utils::PhoneNumber &number, auto offset, auto limit) {
        return retrieve_numbers(dummyHolders, offset, limit);
    };
    NumberHolderMatcher<std::vector, DummyHolder> matcher{std::move(provider), DefaultPageSize};

    SECTION("Incoming e164")
    {
        auto match = matcher.bestMatch(PhoneNumber("+48600123456"));
        REQUIRE(match.has_value());
        REQUIRE(match->getNumber().get() == NumberAPlE164);
    }

    SECTION("Incoming national")
    {
        auto match = matcher.bestMatch(PhoneNumber("600123456", country::Id::UNKNOWN));
        REQUIRE(match.has_value());
        REQUIRE(match->getNumber().get() == NumberAPlNational);
    }

    SECTION("Incoming valid national")
    {
        auto match = matcher.bestMatch(PhoneNumber("600123456", country::Id::POLAND));
        REQUIRE(match.has_value());
        REQUIRE(match->getNumber().get() == NumberAPlE164);
    }
}

TEST_CASE("Number matcher - match incoming (loose)")
{
    auto dummyHolders = make_test_vector({NumberAPlInvalid, NumberBPlInvalid, NumberCPlInvalid});
    auto provider     = [&dummyHolders]([[maybe_unused]] const utils::PhoneNumber &number, auto offset, auto limit) {
        return retrieve_numbers(dummyHolders, offset, limit);
    };
    NumberHolderMatcher<std::vector, DummyHolder> matcher{std::move(provider), DefaultPageSize};

    auto match = matcher.bestMatch(PhoneNumber("+48500123456"));
    REQUIRE(!match.has_value());

    match = matcher.bestMatch(PhoneNumber("+48500123456"), PhoneNumber::Match::POSSIBLE);
    REQUIRE(match.has_value());
    REQUIRE(match->getNumber().get() == NumberBPlNational);
}
