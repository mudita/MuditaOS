#pragma once

namespace msgConstants
{
    constexpr uint32_t singleMessageMaxLen  = 67;
    constexpr uint32_t maxConcatenatedCount = 7;
    constexpr uint32_t maxConcatenatedLen   = singleMessageMaxLen * maxConcatenatedCount;
} // namespace msgConstants