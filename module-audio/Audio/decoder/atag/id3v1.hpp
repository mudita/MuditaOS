#ifndef ATAG_ID3V1_HEADER
#define ATAG_ID3V1_HEADER

#include <string>

namespace atag {
namespace id3v1 {

struct tag
{
    std::string title;
    std::string artist;
    std::string album;
    std::string comment;
    int track_number;
    int year;
    int genre;
};

template<typename Source> bool is_tagged(const Source& s);

template<typename Source> tag parse(const Source& s);

} // namespace id3v1
} // namespace atag

#include "impl/id3v1.ipp"

#endif // ATAG_ID3V1_HEADER
