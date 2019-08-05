#ifndef ATAG_SIMPLE_TAG_HEADER
#define ATAG_SIMPLE_TAG_HEADER

#include "genres.hpp"

#include <string>

namespace atag {

/**
 * Contains generic information about a song. Every tag should be able to provide these
 * information, but this does not mean they will. Thus, if any of the values listed
 * here are empty (empty string or -1 for the integer types), it means no such value was
 * found in the tag.
 */
struct simple_tag
{
    std::string title;
    std::string album;
    std::string artist;
    enum genre genre;
    int track_number;
    int length; // in ms
    int year;
};

inline bool is_valid_tag(const simple_tag& t)
{
    return !t.title.empty() || !t.album.empty() || !t.artist.empty();
}

} // namespace atag

#endif // ATAG_SIMPLE_TAG_HEADER
