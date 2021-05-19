// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TransformFactory.hpp"

#include <Audio/AudioFormat.hpp>

#include "BasicDecimator.hpp"
#include "BasicInterpolator.hpp"
#include "MonoToStereo.hpp"
#include "NullTransform.hpp"
#include "Transform.hpp"
#include "TransformComposite.hpp"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <vector>

#include <cassert>

using audio::transcode::NullTransform;
using audio::transcode::Transform;
using audio::transcode::TransformFactory;

auto TransformFactory::makeTransform(AudioFormat sourceFormat, AudioFormat sinkFormat) const
    -> std::unique_ptr<Transform>
{
    auto transforms = std::vector<std::unique_ptr<Transform>>{};

    if (sourceFormat == sinkFormat) {
        return std::make_unique<NullTransform>();
    }

    if (sourceFormat.getBitWidth() != sinkFormat.getBitWidth()) {
        throw std::runtime_error("Bitwidth conversion is not implemented");
    }

    if (sourceFormat.getSampleRate() != sinkFormat.getSampleRate()) {
        transforms.push_back(getSamplerateTransform(sourceFormat, sinkFormat));
    }

    if (sourceFormat.getChannels() != sinkFormat.getChannels()) {
        transforms.push_back(getChannelsTransform(sourceFormat, sinkFormat));
    }

    assert(!transforms.empty());

    if (transforms.size() > 1) {
        auto transformsListForComposite = std::vector<std::shared_ptr<Transform>>{};
        std::move(std::begin(transforms), std::end(transforms), std::back_inserter(transformsListForComposite));
        return std::make_unique<audio::transcode::TransformComposite>(transformsListForComposite);
    }
    else {
        return std::move(transforms[0]);
    }
}

auto TransformFactory::getSamplerateTransform(AudioFormat sourceFormat, AudioFormat sinkFormat) const
    -> std::unique_ptr<Transform>
{
    static constexpr auto supportedSampleRateCoversionRatio = 2U;
    static constexpr auto supportedBitWidth                 = 16U;
    static constexpr auto supportedChannelCount             = 1U;

    auto sourceRate = sourceFormat.getSampleRate();
    auto sinkRate   = sinkFormat.getSampleRate();

    auto greater = std::max(sourceRate, sinkRate);
    auto lesser  = std::min(sourceRate, sinkRate);

    if (greater % lesser != 0) {
        throw std::invalid_argument("Sample rate conversion is not supported");
    }

    auto ratio = greater / lesser;
    if (ratio != supportedSampleRateCoversionRatio) {
        throw std::invalid_argument("Sample rate conversion is not supported (ratio != 2)");
    }

    if (sourceFormat.getBitWidth() != supportedBitWidth) {
        throw std::invalid_argument("Sample rate conversion with bit width other than 16 is not supported");
    }

    if (sourceFormat.getChannels() != supportedChannelCount) {
        throw std::invalid_argument("Sample rate conversion supported with mono only");
    }

    if (sourceRate > sinkRate) {
        return std::make_unique<audio::transcode::BasicDecimator<std::uint16_t,
                                                                 supportedChannelCount,
                                                                 supportedSampleRateCoversionRatio>>();
    }
    else {
        return std::make_unique<audio::transcode::BasicInterpolator<std::uint16_t,
                                                                    supportedChannelCount,
                                                                    supportedSampleRateCoversionRatio>>();
    }
}

auto TransformFactory::getChannelsTransform(AudioFormat sourceFormat, AudioFormat sinkFormat) const
    -> std::unique_ptr<Transform>
{
    if (sourceFormat.getChannels() == 1 && sinkFormat.getChannels() == 2 && sourceFormat.getBitWidth() == 16) {
        auto transform = std::make_unique<audio::transcode::MonoToStereo>();
        return transform;
    }
    else {
        throw std::invalid_argument("Channels conversion is not supported");
    }
}
