// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "InputTranscodeProxy.hpp"
#include "Transform.hpp"

#include <memory>
#include <utility>

using audio::transcode::InputTranscodeProxy;

InputTranscodeProxy::InputTranscodeProxy(std::shared_ptr<AbstractStream> wrappedStream,
                                         std::shared_ptr<Transform> transform) noexcept
    : StreamProxy(wrappedStream), transform(transform),
      transcodingSpaceSize(transform->getTransformSize(wrappedStream->getInputTraits().blockSize)),
      transcodingSpace(std::make_shared<std::uint8_t[]>(transcodingSpaceSize)), transcodingSpaceSpan{
                                                                                    .data     = transcodingSpace.get(),
                                                                                    .dataSize = transcodingSpaceSize}
{}

bool InputTranscodeProxy::push(const Span &span)
{
    return getWrappedStream().push(transform->transform(span, transcodingSpaceSpan));
}

void InputTranscodeProxy::commit()
{
    transform->transform(peekedSpan, transcodingSpaceSpan);
    getWrappedStream().commit();
    peekedSpan.reset();
}

bool InputTranscodeProxy::peek(Span &span)
{
    auto result = getWrappedStream().peek(span);

    if (result) {
        span.dataSize = transform->transformBlockSizeInversed(span.dataSize);
        peekedSpan    = span;
    }

    return result;
}

auto InputTranscodeProxy::getInputTraits() const noexcept -> Traits
{
    auto originalTraits      = StreamProxy::getInputTraits();
    auto transcodedFormat    = transform->transformFormat(originalTraits.format);
    auto transcodedBlockSize = transform->transformBlockSizeInversed(originalTraits.blockSize);

    return Traits{.blockSize = transcodedBlockSize, .format = transcodedFormat};
}
