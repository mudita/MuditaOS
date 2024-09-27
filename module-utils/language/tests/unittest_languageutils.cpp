// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <gtest/gtest.h>

#include <i18n/i18n.hpp>
#include <LanguageUtils.hpp>

TEST(MinutesGenitive, Polish)
{
    using namespace utils::language;

    utils::setDisplayLanguage("Polski");

    EXPECT_EQ(getCorrectMinutesNumeralForm(0), utils::translate("common_minutes_lower_genitive"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(1), utils::translate("common_minute_lower"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(2), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(3), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(4), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(5), utils::translate("common_minutes_lower_genitive"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(10), utils::translate("common_minutes_lower_genitive"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(11), utils::translate("common_minutes_lower_genitive"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(15), utils::translate("common_minutes_lower_genitive"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(20), utils::translate("common_minutes_lower_genitive"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(22), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(84), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(95), utils::translate("common_minutes_lower_genitive"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(100), utils::translate("common_minutes_lower_genitive"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(101), utils::translate("common_minutes_lower_genitive"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(102), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(1003), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(9999), utils::translate("common_minutes_lower_genitive"));
}

TEST(MinutesGenitive, English)
{
    using namespace utils::language;

    utils::setDisplayLanguage("English");

    EXPECT_EQ(getCorrectMinutesNumeralForm(0), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(1), utils::translate("common_minute_lower"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(5), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(10), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(11), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(15), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(20), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(95), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(100), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(101), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesNumeralForm(9999), utils::translate("common_minutes_lower"));
}

TEST(SecondsGenitive, Polish)
{
    using namespace utils::language;

    utils::setDisplayLanguage("Polski");

    EXPECT_EQ(getCorrectSecondsNumeralForm(0), utils::translate("common_seconds_lower_genitive"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(1), utils::translate("common_second_lower"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(2), utils::translate("common_seconds_lower"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(3), utils::translate("common_seconds_lower"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(4), utils::translate("common_seconds_lower"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(5), utils::translate("common_seconds_lower_genitive"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(10), utils::translate("common_seconds_lower_genitive"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(11), utils::translate("common_seconds_lower_genitive"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(15), utils::translate("common_seconds_lower_genitive"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(20), utils::translate("common_seconds_lower_genitive"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(22), utils::translate("common_seconds_lower"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(84), utils::translate("common_seconds_lower"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(95), utils::translate("common_seconds_lower_genitive"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(100), utils::translate("common_seconds_lower_genitive"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(101), utils::translate("common_seconds_lower_genitive"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(102), utils::translate("common_seconds_lower"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(1003), utils::translate("common_seconds_lower"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(9999), utils::translate("common_seconds_lower_genitive"));
}

TEST(SecondsGenitive, English)
{
    using namespace utils::language;

    utils::setDisplayLanguage("English");

    EXPECT_EQ(getCorrectSecondsNumeralForm(0), utils::translate("common_seconds_lower"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(1), utils::translate("common_second_lower"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(5), utils::translate("common_seconds_lower"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(10), utils::translate("common_seconds_lower"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(11), utils::translate("common_seconds_lower"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(15), utils::translate("common_seconds_lower"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(20), utils::translate("common_seconds_lower"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(95), utils::translate("common_seconds_lower"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(100), utils::translate("common_seconds_lower"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(101), utils::translate("common_seconds_lower"));
    EXPECT_EQ(getCorrectSecondsNumeralForm(9999), utils::translate("common_seconds_lower"));
}

TEST(MinutesAccusative, Polish)
{
    using namespace utils::language;

    utils::setDisplayLanguage("Polski");

    EXPECT_EQ(getCorrectMinutesAccusativeForm(0), utils::translate("common_minutes_lower_genitive"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(1), utils::translate("common_minute_lower_accusative"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(2), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(3), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(4), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(5), utils::translate("common_minutes_lower_genitive"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(10), utils::translate("common_minutes_lower_genitive"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(11), utils::translate("common_minutes_lower_genitive"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(15), utils::translate("common_minutes_lower_genitive"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(20), utils::translate("common_minutes_lower_genitive"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(22), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(84), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(95), utils::translate("common_minutes_lower_genitive"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(100), utils::translate("common_minutes_lower_genitive"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(101), utils::translate("common_minutes_lower_genitive"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(102), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(1003), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(9999), utils::translate("common_minutes_lower_genitive"));
}

TEST(MinutesAccusative, English)
{
    using namespace utils::language;

    utils::setDisplayLanguage("English");

    EXPECT_EQ(getCorrectMinutesAccusativeForm(0), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(1), utils::translate("common_minute_lower"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(5), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(10), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(11), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(15), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(20), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(95), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(100), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(101), utils::translate("common_minutes_lower"));
    EXPECT_EQ(getCorrectMinutesAccusativeForm(9999), utils::translate("common_minutes_lower"));
}

TEST(Multiplicity, Polish)
{
    using namespace utils::language;

    utils::setDisplayLanguage("Polski");

    EXPECT_EQ(getCorrectMultiplicityForm(0), utils::translate("common_multiplicity_many"));
    EXPECT_EQ(getCorrectMultiplicityForm(1), utils::translate("common_multiplicity_once"));
    EXPECT_EQ(getCorrectMultiplicityForm(37), utils::translate("common_multiplicity_many"));
    EXPECT_EQ(getCorrectMultiplicityForm(42), utils::translate("common_multiplicity_many"));
    EXPECT_EQ(getCorrectMultiplicityForm(69), utils::translate("common_multiplicity_many"));
    EXPECT_EQ(getCorrectMultiplicityForm(9999), utils::translate("common_multiplicity_many"));
}
