// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <gtest/gtest.h>

#include <i18n/i18n.hpp>
#include <common/LanguageUtils.hpp>

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
