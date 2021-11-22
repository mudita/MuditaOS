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
      transcodingSpaceSize(transform->transformBlockSizeInverted(wrappedStream->getInputTraits().blockSize)),
      transcodingSpace(std::make_unique<std::uint8_t[]>(transcodingSpaceSize)), transcodingSpaceSpan{
                                                                                    .data     = transcodingSpace.get(),
                                                                                    .dataSize = transcodingSpaceSize}
{}

bool InputTranscodeProxy::push(const Span &span)
{
    Span outputSpan;

    if (!getWrappedStream().reserve(outputSpan)) {
        return false;
    }

    transform->transform(span, outputSpan);
    getWrappedStream().commit();

    return true;
}

bool InputTranscodeProxy::push(void *data, std::size_t dataSize)
{
    return push(Span{.data = reinterpret_cast<std::uint8_t *>(data), .dataSize = dataSize});
}

void InputTranscodeProxy::commit()
{
    if (isReserved) {
        transform->transform(transcodingSpaceSpan, reservedSpan);
        getWrappedStream().commit();
        reservedSpan.reset();
        isReserved = false;
    }
}

bool InputTranscodeProxy::reserve(Span &inputSpan)
{
    Span span;
    auto result = getWrappedStream().reserve(span);

    if (result) {
        reservedSpan = span;
        inputSpan    = transcodingSpaceSpan;
        isReserved   = true;
    }

    return result;
}

auto InputTranscodeProxy::getInputTraits() const noexcept -> Traits
{
    auto originalTraits      = StreamProxy::getInputTraits();
    auto transcodedFormat    = transform->transformFormat(originalTraits.format);
    auto transcodedBlockSize = transform->transformBlockSizeInverted(originalTraits.blockSize);

    return Traits{.blockSize = transcodedBlockSize, .format = transcodedFormat};
}
