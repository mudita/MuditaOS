#include "TransformFactory.hpp"

#include <Audio/AudioFormat.hpp>

#include "Transform.hpp"
#include "MonoToStereo.hpp"
#include "BasicDecimator.hpp"
#include "BasicInterpolator.hpp"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <vector>

using audio::transcode::Transform;
using audio::transcode::TransformFactory;

auto TransformFactory::makeTransform(AudioFormat sourceFormat, AudioFormat sinkFormat) const
    -> std::unique_ptr<Transform>
{
    auto transforms = std::vector<std::unique_ptr<Transform>>{};

    // TODO: add null transform if equal
    if (sourceFormat == sinkFormat) {
        return nullptr;
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

    // create composite
    if (transforms.size() != 1) {
        return nullptr;
    }

    return std::move(transforms[0]);
}

auto TransformFactory::getSamplerateTransform(AudioFormat sourceFormat, AudioFormat sinkFormat) const
    -> std::unique_ptr<Transform>
{
    auto sourceRate = sourceFormat.getSampleRate();
    auto sinkRate   = sinkFormat.getSampleRate();

    auto greater = std::max(sourceRate, sinkRate);
    auto lesser  = std::min(sourceRate, sinkRate);

    if (greater % lesser != 0) {
        throw std::invalid_argument("Sample rate conversion is not supported");
    }

    auto ratio = greater / lesser;
    if (ratio != 2) {
        throw std::invalid_argument("Sample rate conversion is not supported (ratio != 2)");
    }

    if (sourceFormat.getBitWidth() != 16) {
        throw std::invalid_argument("Sample rate conversion with bit width other than 16 is not supported");
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
