// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <algorithm>
#include "RandomStringGenerator.hpp"

std::string RandomStringGenerator::getRandomString()
{
    const size_t length = lengthDist(rng);
    std::string str(length, 0);
    std::generate_n(str.begin(), length, [this]() { return charSet[charDist(rng)]; });
    return str;
}

std::vector<std::string> RandomStringGenerator::createRandomStringVector(size_t size)
{
    std::vector<std::string> vec(size);
    std::generate_n(vec.begin(), size, [this]() { return getRandomString(); });
    return vec;
}
