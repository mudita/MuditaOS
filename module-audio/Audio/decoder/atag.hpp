#ifndef ATAG_HEADER
#define ATAG_HEADER

#include <vector>
#include <string>

#include "atag/simple_tag.hpp"
#include "atag/genres.hpp"
#include "atag/id3v1.hpp"
#include "atag/id3v2.hpp"
#include "atag/flac.hpp"
#include "atag/ape.hpp"

namespace atag {

/**
 * Parses an APE, ID3v1, or ID3v2 tagged audio file and produces a generic tag with
 * only the most important details about the file.
 */
template<typename Source> simple_tag parse(const Source& s);

/**
 * A set of comparators which can be used to sort collections of tags by track number,
 * song title, album title, artist name etc.
 *
 * Example:
 * ```
 * std::vector<atag::simple_tag> tags;
 * // or
 * std::vector<atag::flac::tag> tags;
 * // ...
 * std::sort(tags.begin(), tags.end(), atag::order::track_number());
 * std::sort(tags.begin(), tags.end(), atag::order::title());
 * ```
 */
namespace order {

struct track_number
{
    template<typename Tag>
    bool operator()(const Tag& a, const Tag& b) const noexcept
    {
        return a.track_number < b.track_number;
    }
};

struct title
{
    template<typename Tag>
    bool operator()(const Tag& a, const Tag& b) const noexcept
    {
        return a.title < b.title;
    }
};

struct album
{
    template<typename Tag>
    bool operator()(const Tag& a, const Tag& b) const noexcept
    {
        return a.album < b.album;
    }
};

struct year
{
    template<typename Tag>
    bool operator()(const Tag& a, const Tag& b) const noexcept
    {
        return a.year < b.year;
    }
};

struct artist
{
    template<typename Tag>
    bool operator()(const Tag& a, const Tag& b) const noexcept
    {
        return a.artist < b.artist;
    }
};

} // namespace order
} // namespace atag

#include "atag/impl/atag.ipp"

#endif // ATAG_HEADER
