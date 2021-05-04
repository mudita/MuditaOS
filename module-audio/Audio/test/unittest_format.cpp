// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <gtest/gtest.h>

#include <Audio/AudioFormat.hpp>

#include <algorithm>
#include <string>
#include <vector>

using audio::AudioFormat;

TEST(AudioFormat, Bitrate)
{
    AudioFormat format{44100, 16, 2};

    EXPECT_EQ(format.getBitrate(), 1411200);
}

TEST(AudioFormat, Compare)
{
    AudioFormat first{44100, 16, 2};
    AudioFormat second{44100, 32, 1};
    AudioFormat third{48000, 16, 2};

    EXPECT_TRUE(first == AudioFormat(44100, 16, 2));
    EXPECT_TRUE(first != second);
    EXPECT_TRUE(first < third);
    EXPECT_FALSE(second > third);
    EXPECT_TRUE(first <= third);
    EXPECT_TRUE(first >= second);
}

TEST(AudioFormat, ToString)
{
    AudioFormat format{44100, 16, 2};

    EXPECT_EQ(std::string{"AudioFormat{44100,16,2}"}, format.toString());
}

TEST(AudioFormat, MakeMatrix)
{
    auto matrix = AudioFormat::makeMatrix({16000, 44100, 48000}, {16, 24}, {1, 2});

    ASSERT_EQ(matrix.size(), 12);

    EXPECT_TRUE(std::find(std::begin(matrix), std::end(matrix), AudioFormat{16000, 16, 1}) != std::end(matrix));
    EXPECT_TRUE(std::find(std::begin(matrix), std::end(matrix), AudioFormat{44100, 16, 1}) != std::end(matrix));
    EXPECT_TRUE(std::find(std::begin(matrix), std::end(matrix), AudioFormat{48000, 16, 1}) != std::end(matrix));

    EXPECT_TRUE(std::find(std::begin(matrix), std::end(matrix), AudioFormat{16000, 24, 1}) != std::end(matrix));
    EXPECT_TRUE(std::find(std::begin(matrix), std::end(matrix), AudioFormat{44100, 24, 1}) != std::end(matrix));
    EXPECT_TRUE(std::find(std::begin(matrix), std::end(matrix), AudioFormat{48000, 24, 1}) != std::end(matrix));

    EXPECT_TRUE(std::find(std::begin(matrix), std::end(matrix), AudioFormat{16000, 16, 2}) != std::end(matrix));
    EXPECT_TRUE(std::find(std::begin(matrix), std::end(matrix), AudioFormat{44100, 16, 2}) != std::end(matrix));
    EXPECT_TRUE(std::find(std::begin(matrix), std::end(matrix), AudioFormat{48000, 16, 2}) != std::end(matrix));

    EXPECT_TRUE(std::find(std::begin(matrix), std::end(matrix), AudioFormat{16000, 24, 2}) != std::end(matrix));
    EXPECT_TRUE(std::find(std::begin(matrix), std::end(matrix), AudioFormat{44100, 24, 2}) != std::end(matrix));
    EXPECT_TRUE(std::find(std::begin(matrix), std::end(matrix), AudioFormat{48000, 24, 2}) != std::end(matrix));
}

TEST(AudioFormat, Validity)
{
    EXPECT_FALSE(AudioFormat(44100, 16, 0).isValid());
    EXPECT_FALSE(AudioFormat(44100, 16, 0).isValid());
    EXPECT_FALSE(AudioFormat(48000, 0, 3).isValid());
    EXPECT_FALSE(AudioFormat(0, 16, 3).isValid());
    EXPECT_FALSE(AudioFormat().isValid());

    EXPECT_TRUE(AudioFormat(44100, 16, 2).isValid());
}

TEST(AudioFormat, Null)
{
    EXPECT_TRUE(AudioFormat().isNull());
    EXPECT_FALSE(AudioFormat(44100, 16, 0).isNull());
    EXPECT_FALSE(AudioFormat(44100, 16, 2).isNull());
}

TEST(AudioFormat, DurationToBytes)
{
    auto format = AudioFormat(44100, 16, 2);

    EXPECT_EQ(format.microsecondsToBytes(std::chrono::microseconds(0)), 0);
    EXPECT_EQ(format.microsecondsToBytes(std::chrono::seconds(1)), 176400);
}

TEST(AudioFormat, BytesToDuration)
{
    auto format = AudioFormat(8000, 8, 1);

    EXPECT_EQ(format.bytesToMicroseconds(0).count(), 0);
    EXPECT_EQ(format.bytesToMicroseconds(1).count(), 125);
}
