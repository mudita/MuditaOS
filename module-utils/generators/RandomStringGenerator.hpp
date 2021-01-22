// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <random>
#include <string>
#include <vector>

class RandomStringGenerator
{
  public:
    RandomStringGenerator(size_t minLength = minRandomStringLength, size_t maxLength = maxRandomStringLength)
        : minLength(minLength), maxLength(maxLength), lengthDist(minLength, maxLength)
    {}

    std::string getRandomString();
    std::vector<std::string> createRandomStringVector(size_t size);

  private:
    std::uniform_int_distribution<> charDist{0, sizeof(charSet) - 1};
    std::default_random_engine rng{std::random_device{}()};
    const size_t minLength;
    const size_t maxLength;
    std::uniform_int_distribution<> lengthDist;

    static constexpr auto minRandomStringLength = 1;
    static constexpr auto maxRandomStringLength = 25;
    static constexpr char charSet[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
                                       'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
                                       'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
                                       'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
};
