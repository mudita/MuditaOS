// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestStream.hpp"
#include "MockStream.hpp"

#include <Audio/AbstractStream.hpp>
#include <Audio/AudioFormat.hpp>
#include <Audio/transcode/InputTranscodeProxy.hpp>
#include <Audio/transcode/Transform.hpp>
#include <Audio/transcode/MonoToStereo.hpp>
#include <Audio/transcode/TransformComposite.hpp>
#include <Audio/transcode/BasicInterpolator.hpp>
#include <Audio/transcode/BasicDecimator.hpp>
#include <Audio/transcode/NullTransform.hpp>
#include <Audio/transcode/TransformFactory.hpp>

#include <cstdlib>

using ::audio::transcode::NullTransform;
using ::testing::_;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::audio::MockStream;
using ::testing::audio::MockStreamEventListener;

constexpr std::size_t testStreamSize = 8;

class InverseTransform : public audio::transcode::Transform
{
  public:
    auto transform(const Span &input, const Span &output) const -> Span override
    {
        for (std::size_t i = 0; i < input.dataSize; i++) {
            output.data[i] = ~input.data[i];
        }
        return output;
    }

    auto transformBlockSize(std::size_t inputBufferSize) const noexcept -> std::size_t override
    {
        return inputBufferSize;
    }

    auto transformBlockSizeInverted(std::size_t outputBufferSize) const noexcept -> std::size_t override
    {
        return outputBufferSize;
    }

    auto validateInputFormat(const audio::AudioFormat &inputFormat) const noexcept -> bool override
    {
        return true;
    }

    auto transformFormat(const audio::AudioFormat &inputFormat) const noexcept -> audio::AudioFormat override
    {
        return inputFormat;
    }
};

TEST(Transcode, Reset)
{
    auto mock          = std::make_shared<MockStream>();
    auto nullTransform = std::make_shared<NullTransform>();

    EXPECT_CALL(*mock, getInputTraits);
    audio::transcode::InputTranscodeProxy proxy(std::static_pointer_cast<::audio::AbstractStream>(mock), nullTransform);

    EXPECT_CALL(*mock, reset()).Times(1);

    proxy.reset();
}

TEST(Transcode, Push)
{
    static std::uint8_t testData[testStreamSize]     = {0, 1, 2, 3, 4, 5, 6, 7};
    static std::uint8_t invertedData[testStreamSize] = {255, 254, 253, 252, 251, 250, 249, 248};
    auto inv                                         = std::make_shared<InverseTransform>();
    auto stream                                      = std::make_shared<::testing::audio::TestStream>(testStreamSize);
    ::audio::transcode::InputTranscodeProxy transform(std::static_pointer_cast<::audio::AbstractStream>(stream), inv);

    transform.push(::audio::AbstractStream::Span{testData, testStreamSize});
    EXPECT_TRUE(stream->checkData(::audio::AbstractStream::Span{invertedData, testStreamSize}));
}

TEST(Transcode, FailedPeek)
{
    auto nullTransform = std::make_shared<NullTransform>();
    auto mockStream    = std::make_shared<MockStream>();

    EXPECT_CALL(*mockStream, getInputTraits);
    ::audio::transcode::InputTranscodeProxy transform(std::static_pointer_cast<::audio::AbstractStream>(mockStream),
                                                      nullTransform);
    ::audio::AbstractStream::Span dataSpan;

    EXPECT_CALL(*mockStream, peek).Times(1).WillOnce(Return(false));
    EXPECT_EQ(transform.peek(dataSpan), false);
}

TEST(Transcode, Commit)
{
    static std::uint8_t invertedData[testStreamSize] = {255, 254, 253, 252, 251, 250, 249, 248};
    auto inv                                         = std::make_shared<InverseTransform>();
    auto stream                                      = std::make_shared<::testing::audio::TestStream>(testStreamSize);
    ::audio::transcode::InputTranscodeProxy transform(std::static_pointer_cast<::audio::AbstractStream>(stream), inv);
    ::audio::AbstractStream::Span span;

    stream->setData(0);

    ASSERT_TRUE(transform.reserve(span));
    ASSERT_TRUE(span.data != nullptr);
    ASSERT_TRUE(span.dataSize == testStreamSize);

    for (std::size_t i = 0; i < span.dataSize; ++i) {
        span.data[i] = i;
    }

    transform.commit();
    EXPECT_TRUE(stream->checkData(::audio::AbstractStream::Span{invertedData, testStreamSize}));

    // no conversion on commit with no reserve
    stream->commit();
    EXPECT_TRUE(stream->checkData(::audio::AbstractStream::Span{invertedData, testStreamSize}));
}

TEST(Transcode, ReserveShrinking)
{
    static std::uint16_t streamData[8];
    auto streamDataSpan = ::audio::AbstractStream::Span{.data     = reinterpret_cast<std::uint8_t *>(streamData),
                                                        .dataSize = sizeof(streamData)};

    auto decimatorTransform = std::make_shared<::audio::transcode::BasicDecimator<std::uint16_t, 1, 2>>();
    auto mockStream         = std::make_shared<MockStream>();

    EXPECT_CALL(*mockStream, getInputTraits)
        .WillRepeatedly(Return(::audio::AbstractStream::Traits{.blockSize = streamDataSpan.dataSize,
                                                               .format    = audio::AudioFormat(8000, 16, 1)}));

    auto transcodingProxy = ::audio::transcode::InputTranscodeProxy(mockStream, decimatorTransform);

    EXPECT_CALL(*mockStream, reserve(_)).WillOnce(DoAll(SetArgReferee<0>(streamDataSpan), Return(true)));

    ::audio::AbstractStream::Span span;
    transcodingProxy.reserve(span);

    EXPECT_EQ(span.dataSize, 16 * sizeof(std::uint16_t));

    auto buf = reinterpret_cast<std::uint16_t *>(span.data);
    for (unsigned int i = 0; i < 16; i++) {
        buf[i] = i;
    }

    EXPECT_CALL(*mockStream, commit);

    transcodingProxy.commit();

    static std::uint16_t expectedData[8] = {0, 2, 4, 6, 8, 10, 12, 14};
    EXPECT_TRUE(memcmp(expectedData, streamData, sizeof(expectedData)) == 0);
}

TEST(Transcode, ReserveBloating)
{
    static std::uint16_t streamData[8];
    auto streamDataSpan = ::audio::AbstractStream::Span{.data     = reinterpret_cast<std::uint8_t *>(streamData),
                                                        .dataSize = sizeof(streamData)};

    auto interpolatorTransform = std::make_shared<::audio::transcode::BasicInterpolator<std::uint16_t, 1, 2>>();
    auto mockStream            = std::make_shared<MockStream>();

    EXPECT_CALL(*mockStream, getInputTraits)
        .WillRepeatedly(Return(::audio::AbstractStream::Traits{.blockSize = streamDataSpan.dataSize,
                                                               .format    = audio::AudioFormat(8000, 16, 1)}));

    auto transcodingProxy = ::audio::transcode::InputTranscodeProxy(mockStream, interpolatorTransform);

    EXPECT_CALL(*mockStream, reserve(_)).WillOnce(DoAll(SetArgReferee<0>(streamDataSpan), Return(true)));

    ::audio::AbstractStream::Span span;
    transcodingProxy.reserve(span);

    EXPECT_EQ(span.dataSize, 4 * sizeof(std::uint16_t));

    auto buf = reinterpret_cast<std::uint16_t *>(span.data);
    for (unsigned int i = 0; i < 4; i++) {
        buf[i] = i;
    }

    EXPECT_CALL(*mockStream, commit);

    transcodingProxy.commit();

    static std::uint16_t expectedData[8] = {0, 0, 1, 1, 2, 2, 3, 3};
    EXPECT_TRUE(memcmp(expectedData, streamData, sizeof(expectedData)) == 0);
}

TEST(Transcode, Traits)
{
    auto testFormat = ::audio::AudioFormat(44100, 16, 1);
    auto m2s        = std::make_shared<::audio::transcode::MonoToStereo>();
    auto mockStream = std::make_shared<MockStream>();

    EXPECT_CALL(*mockStream, getInputTraits)
        .Times(1)
        .WillOnce(Return(::audio::AbstractStream::Traits{.blockSize = 128, .format = testFormat}));

    ::audio::transcode::InputTranscodeProxy proxy(std::static_pointer_cast<::audio::AbstractStream>(mockStream), m2s);

    EXPECT_CALL(*mockStream, getInputTraits)
        .Times(1)
        .WillOnce(Return(::audio::AbstractStream::Traits{.blockSize = 128, .format = testFormat}));

    EXPECT_CALL(*mockStream, getOutputTraits)
        .Times(1)
        .WillOnce(Return(::audio::AbstractStream::Traits{.blockSize = 128, .format = testFormat}));

    auto proxyInputTraits = proxy.getInputTraits();

    EXPECT_EQ(proxyInputTraits.blockSize, 64);
    EXPECT_EQ(proxyInputTraits.format.getChannels(), 2);
    EXPECT_EQ(proxyInputTraits.format.getSampleRate(), 44100);
    EXPECT_EQ(proxyInputTraits.format.getBitWidth(), 16);

    auto proxyOutputTraits = proxy.getOutputTraits();

    EXPECT_EQ(proxyOutputTraits.blockSize, 128);
    EXPECT_EQ(proxyOutputTraits.format.getChannels(), 1);
    EXPECT_EQ(proxyOutputTraits.format.getSampleRate(), 44100);
    EXPECT_EQ(proxyOutputTraits.format.getBitWidth(), 16);
}

TEST(Transcode, ListenersWrap)
{
    auto mock          = std::make_shared<MockStream>();
    auto nullTransform = std::make_shared<NullTransform>();
    MockStreamEventListener listener;

    EXPECT_CALL(*mock, getInputTraits);
    audio::transcode::InputTranscodeProxy proxy(std::static_pointer_cast<::audio::AbstractStream>(mock), nullTransform);

    EXPECT_CALL(*mock, registerListener(&listener)).Times(1);

    proxy.registerListener(&listener);

    EXPECT_CALL(*mock, unregisterListeners(&listener)).Times(1);

    proxy.unregisterListeners(&listener);
}

TEST(Transform, MonoToStereo)
{
    audio::transcode::MonoToStereo m2s;
    static std::uint16_t inputBuffer[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    static std::uint16_t outputBuffer[16];
    auto input  = ::audio::AbstractStream::Span{.data     = reinterpret_cast<std::uint8_t *>(&inputBuffer[0]),
                                               .dataSize = sizeof(inputBuffer)};
    auto output = ::audio::AbstractStream::Span{.data     = reinterpret_cast<std::uint8_t *>(&outputBuffer[0]),
                                                .dataSize = sizeof(outputBuffer)};

    ASSERT_EQ(input.dataSize, 16);
    ASSERT_EQ(output.dataSize, 32);

    EXPECT_TRUE(m2s.validateInputFormat(audio::AudioFormat(44100, 16, 1)));
    EXPECT_FALSE(m2s.validateInputFormat(audio::AudioFormat(44100, 16, 0)));
    EXPECT_FALSE(m2s.validateInputFormat(audio::AudioFormat(44100, 16, 2)));

    auto outputFormat = m2s.transformFormat(audio::AudioFormat(44100, 16, 1));
    EXPECT_EQ(outputFormat, audio::AudioFormat(44100, 16, 2));

    static std::uint16_t expectedResult[16] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7};
    ASSERT_EQ(m2s.transform(input, output), output);
    ASSERT_EQ(memcmp(output.data, expectedResult, 32), 0);
}

TEST(Transform, Composite)
{
    auto m2s = std::make_shared<audio::transcode::MonoToStereo>();
    auto inv = std::make_shared<InverseTransform>();
    audio::transcode::TransformComposite composite(
        std::vector<std::shared_ptr<::audio::transcode::Transform>>{m2s, inv});
    static std::uint16_t inputBuffer[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    static std::uint16_t outputBuffer[16];
    static std::uint16_t expectedResult[] = {
        0xFFFF,
        0xFFFF,
        0xFFFE,
        0xFFFE,
        0xFFFD,
        0xFFFD,
        0xFFFC,
        0xFFFC,
        0xFFFB,
        0xFFFB,
        0xFFFA,
        0xFFFA,
        0xFFF9,
        0xFFF9,
        0xFFF8,
        0xFFF8,
    };

    auto input  = ::audio::AbstractStream::Span{.data     = reinterpret_cast<std::uint8_t *>(&inputBuffer[0]),
                                               .dataSize = sizeof(inputBuffer)};
    auto output = ::audio::AbstractStream::Span{.data     = reinterpret_cast<std::uint8_t *>(&outputBuffer[0]),
                                                .dataSize = sizeof(outputBuffer)};

    ASSERT_EQ(output.dataSize, composite.transformBlockSize(sizeof(inputBuffer)));
    auto result = composite.transform(input, output);
    ASSERT_EQ(result.dataSize, sizeof(expectedResult));
    ASSERT_EQ(memcmp(result.data, expectedResult, sizeof(expectedResult)), 0);

    EXPECT_TRUE(composite.validateInputFormat(audio::AudioFormat(44100, 16, 1)));
    EXPECT_FALSE(composite.validateInputFormat(audio::AudioFormat(44100, 16, 0)));
    EXPECT_FALSE(composite.validateInputFormat(audio::AudioFormat(44100, 16, 2)));

    auto outputFormat = composite.transformFormat(audio::AudioFormat(44100, 16, 1));
    EXPECT_EQ(outputFormat, audio::AudioFormat(44100, 16, 2));
    auto outputBlockSize = composite.transformBlockSize(sizeof(inputBuffer));
    EXPECT_EQ(outputBlockSize, 2 * sizeof(inputBuffer));
}

TEST(Transform, BasicInterpolator)
{
    audio::transcode::BasicInterpolator<std::uint16_t, 2, 2> interp2;
    audio::transcode::BasicInterpolator<std::uint32_t, 1, 3> interp3;

    EXPECT_EQ(interp2.transformBlockSize(128), 256);
    EXPECT_EQ(interp3.transformBlockSize(100), 300);

    auto format        = audio::AudioFormat{8000, 16, 2};
    auto outputFormat2 = interp2.transformFormat(format);
    auto outputFormat3 = interp3.transformFormat(format);

    EXPECT_EQ(outputFormat2.getSampleRate(), 16000);
    EXPECT_EQ(outputFormat3.getSampleRate(), 24000);

    EXPECT_EQ(outputFormat2.getBitWidth(), 16);
    EXPECT_EQ(outputFormat3.getBitWidth(), 16);

    EXPECT_EQ(outputFormat2.getChannels(), 2);
    EXPECT_EQ(outputFormat3.getChannels(), 2);

    EXPECT_TRUE(interp2.validateInputFormat(format));
    EXPECT_FALSE(interp3.validateInputFormat(format));

    std::uint16_t inputBuffer[8]          = {1, 2, 3, 4, 0, 0, 0, 0};
    static const uint16_t expectBuffer[8] = {1, 2, 1, 2, 3, 4, 3, 4};
    auto inputSpan      = ::audio::AbstractStream::Span{.data     = reinterpret_cast<uint8_t *>(inputBuffer),
                                                   .dataSize = 4 * sizeof(std::uint16_t)};
    auto transformSpace = ::audio::AbstractStream::Span{.data     = reinterpret_cast<uint8_t *>(inputBuffer),
                                                        .dataSize = 8 * sizeof(std::uint16_t)};
    auto outputSpan     = interp2.transform(inputSpan, transformSpace);

    EXPECT_EQ(outputSpan.dataSize, sizeof(uint16_t) * 8);
    EXPECT_EQ(memcmp(outputSpan.data, expectBuffer, outputSpan.dataSize), 0);
}

TEST(Transform, BasicDecimator)
{
    audio::transcode::BasicDecimator<std::uint16_t, 2, 2> decim2;

    EXPECT_EQ(decim2.transformBlockSize(128), 64);

    auto format        = audio::AudioFormat{16000, 16, 2};
    auto outputFormat2 = decim2.transformFormat(format);

    EXPECT_EQ(outputFormat2.getSampleRate(), 8000);
    EXPECT_EQ(outputFormat2.getBitWidth(), 16);
    EXPECT_EQ(outputFormat2.getChannels(), 2);

    auto invalidFormat = audio::AudioFormat{16000, 8, 2};
    EXPECT_TRUE(decim2.validateInputFormat(format));
    EXPECT_FALSE(decim2.validateInputFormat(invalidFormat));

    std::uint16_t inputBuffer[8]          = {1, 2, 1, 2, 3, 4, 3, 4};
    static const uint16_t expectBuffer[8] = {1, 2, 3, 4, 0, 0, 0, 0};
    auto inputSpan  = ::audio::AbstractStream::Span{.data     = reinterpret_cast<uint8_t *>(inputBuffer),
                                                   .dataSize = 8 * sizeof(std::uint16_t)};
    auto outputSpan = decim2.transform(inputSpan, inputSpan);

    EXPECT_EQ(outputSpan.dataSize, sizeof(uint16_t) * 4);
    EXPECT_EQ(memcmp(outputSpan.data, expectBuffer, outputSpan.dataSize), 0);
}

TEST(Transform, FactorySampleRateInterpolator)
{
    auto factory      = ::audio::transcode::TransformFactory();
    auto sourceFormat = ::audio::AudioFormat{8000, 16, 1};
    auto sinkFormat   = ::audio::AudioFormat{16000, 16, 1};

    auto transform = factory.makeTransform(sourceFormat, sinkFormat);

    EXPECT_STREQ(typeid(*transform).name(), typeid(::audio::transcode::BasicInterpolator<std::uint16_t, 1, 2>).name());
}

TEST(Transform, FactorySampleRateDecimator)
{
    auto factory      = ::audio::transcode::TransformFactory();
    auto sourceFormat = ::audio::AudioFormat{16000, 16, 1};
    auto sinkFormat   = ::audio::AudioFormat{8000, 16, 1};

    auto transform = factory.makeTransform(sourceFormat, sinkFormat);

    EXPECT_STREQ(typeid(*transform).name(), typeid(::audio::transcode::BasicDecimator<std::uint16_t, 1, 2>).name());
    EXPECT_EQ(transform->transformFormat(sourceFormat), sinkFormat);
}

TEST(Tranform, FactoryNullTransform)
{
    auto factory      = ::audio::transcode::TransformFactory();
    auto sourceFormat = ::audio::AudioFormat{8000, 16, 1};
    auto sinkFormat   = ::audio::AudioFormat{8000, 16, 1};

    auto transform = factory.makeTransform(sourceFormat, sinkFormat);

    EXPECT_STREQ(typeid(*transform).name(), typeid(::audio::transcode::NullTransform).name());
    EXPECT_EQ(transform->transformFormat(sourceFormat), sinkFormat);
}

TEST(Transform, FactoryMonoToStereo)
{
    auto factory      = ::audio::transcode::TransformFactory();
    auto sourceFormat = ::audio::AudioFormat{8000, 16, 1};
    auto sinkFormat   = ::audio::AudioFormat{8000, 16, 2};

    auto transform = factory.makeTransform(sourceFormat, sinkFormat);

    EXPECT_STREQ(typeid(*transform).name(), typeid(::audio::transcode::MonoToStereo).name());
    EXPECT_EQ(transform->transformFormat(sourceFormat), sinkFormat);
}

TEST(Transform, FactoryComposite)
{
    auto factory      = ::audio::transcode::TransformFactory();
    auto sourceFormat = ::audio::AudioFormat{16000, 16, 1};
    auto sinkFormat   = ::audio::AudioFormat{32000, 16, 2};

    auto transform = factory.makeTransform(sourceFormat, sinkFormat);

    EXPECT_STREQ(typeid(*transform).name(), typeid(::audio::transcode::TransformComposite).name());
    EXPECT_EQ(transform->transformFormat(sourceFormat), sinkFormat);
}

TEST(Transform, FactoryErrors)
{
    auto factory = ::audio::transcode::TransformFactory();
    EXPECT_THROW(factory.makeTransform(::audio::AudioFormat{16000, 16, 1}, ::audio::AudioFormat{16000, 24, 1}),
                 std::runtime_error);
    EXPECT_THROW(factory.makeTransform(::audio::AudioFormat{44100, 16, 1}, ::audio::AudioFormat{48000, 16, 1}),
                 std::invalid_argument);
    EXPECT_THROW(factory.makeTransform(::audio::AudioFormat{8000, 16, 1}, ::audio::AudioFormat{24000, 16, 1}),
                 std::invalid_argument);
    EXPECT_THROW(factory.makeTransform(::audio::AudioFormat{16000, 32, 1}, ::audio::AudioFormat{8000, 32, 1}),
                 std::invalid_argument);
    EXPECT_THROW(factory.makeTransform(::audio::AudioFormat{8000, 16, 2}, ::audio::AudioFormat{16000, 16, 2}),
                 std::invalid_argument);

    // channel conversions
    EXPECT_THROW(factory.makeTransform(::audio::AudioFormat{8000, 16, 1}, ::audio::AudioFormat{8000, 16, 3}),
                 std::invalid_argument);
    EXPECT_THROW(factory.makeTransform(::audio::AudioFormat{8000, 16, 2}, ::audio::AudioFormat{8000, 16, 1}),
                 std::invalid_argument);
    EXPECT_THROW(factory.makeTransform(::audio::AudioFormat{8000, 32, 1}, ::audio::AudioFormat{8000, 32, 2}),
                 std::invalid_argument);
}
