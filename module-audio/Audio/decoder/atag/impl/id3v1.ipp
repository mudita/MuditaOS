#ifndef ATAG_ID3V1_IMPL_HEADER
#define ATAG_ID3V1_IMPL_HEADER

#include "../detail/type_traits.hpp"
#include "../id3v1.hpp"

#include <algorithm>

namespace atag {
namespace id3v1 {

enum { tag_size = 128 };

template<typename Source> bool is_tagged(const Source& s)
{
    static_assert(detail::is_source<Source>::value, "Source requirements not met");

    if(s.size() < tag_size) { return false; }
    const auto tag_begin = &s[s.size() - tag_size];
    return std::equal(tag_begin, tag_begin + 3, "TAG");
}

template<typename Source> tag parse(const Source& s)
{
    static_assert(detail::is_source<Source>::value, "Source requirements not met");

    if(!is_tagged(s)) { return {}; }

    auto pos = &s[s.size() - tag_size] + 3;
    tag tag;

    tag.title = std::string(pos, std::find(pos, pos + 30, 0));
    pos += 30;
    tag.artist = std::string(pos, std::find(pos, pos + 30, 0));
    pos += 30;
    tag.album = std::string(pos, std::find(pos, pos + 30, 0));
    pos += 30;
    tag.comment = std::string(pos, std::find(pos, pos + 28, 0));
    pos += 29;
    tag.track_number = *pos;
    pos += 1;
    tag.year = std::atoi(pos);
    pos += 4;
    tag.genre = *pos;

    return tag;
}

} // namespace id3v1
} // namespace atag

#endif // ATAG_ID3V1_IMPL_HEADER
