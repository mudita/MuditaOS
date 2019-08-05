#ifndef ATAG_IMPL_HEADER
#define ATAG_IMPL_HEADER

#include "../../atag.hpp"

#include <algorithm>
#include <cstdlib>

namespace atag {

template<typename Source>
simple_tag parse(const Source& s)
{
    simple_tag t;
    if(id3v2::is_tagged(s))
    {
        t = id3v2::simple_parse(s);
    }
    else if(flac::is_tagged(s))
    {
        flac::tag f = flac::parse(s);
        t.title = std::move(f.title);
        t.album = std::move(f.album);
        t.artist = std::move(f.artist);
        //// TODO
        //t.genre = std::move(d.genre);
        t.year = f.year;
        t.track_number = f.track_number;
    }
    else if(id3v1::is_tagged(s))
    {
        id3v1::tag d = id3v1::parse(s);
        t.title = std::move(d.title);
        t.album = std::move(d.album);
        t.artist = std::move(d.artist);
        //t.genre = std::move(d.title);
        t.track_number = d.track_number;
        t.year = d.year;
    }
    /*
    if(ape::is_tagged(s))
    {
        // TODO
    }
    */
    return t;
}

} // namespace atag

#endif // ATAG_IMPL_HEADER
