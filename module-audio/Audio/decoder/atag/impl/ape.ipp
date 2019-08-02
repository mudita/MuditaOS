#ifndef ATAG_APE_IMPL
#define ATAG_APE_IMPL

#include "../detail/type_traits.hpp"
#include "../detail/io_util.hpp"
#include "../ape.hpp"

#include <algorithm>

namespace atag {
namespace ape {

struct header
{
    enum
    {
        contains_header = 1 << 31,
        contains_footer = 1 << 30,
        this_is_the_header = 1 << 30,
        read_only = 1,
    };

    int version;
    int tag_size;
    int num_items;
    uint32_t flags;

    enum { size = 32 };
};

template<typename Ptr>
header parse_header(Ptr p)
{
    header h;
    h.version = detail::parse_be<int>(p + 8);
    h.tag_size = detail::parse_be<int>(p + 12);
    h.num_items = detail::parse_be<int>(p + 16);
    h.flags = detail::parse_be<uint32_t>(p + 20);
    return h;
}

inline bool is_header_valid(const header& h)
{
    // TODO
    return h.size > 0
        && h.num_items > 0;
}

struct item_header
{
    enum {
        item_encoded_as_utf8 = 0b000,
        item_contains_binary_info = 0b010,
        item_contains_external_info_locator = 0b100,
        reserved = 0b110,
        read_only = 1,
    };

    int value_size;
    uint32_t flags;
};

template<typename Ptr>
item_header parse_item_header(Ptr p)
{
    item_header h;
    h.value_size = detail::parse_be<int>(p);
    h.flags = detail::parse_be<int>(p + 4);
    return h;
}

template<typename Source>
int find_tag_start(const Source& s)
{
    const auto matches_header = [](const auto* p)
    {
        if(!std::equal(p, p + 8, "APETAGEX")) { return false; }
        for(auto i = 0; i < 8; ++i)
        {
            if(p[i+24] != 0) { return false; }
        }
        return true;
    };

    if(matches_header(&s[0]))
    {
        return 0;
    }
    else if(matches_header(&s[s.size() - header::size]))
    {
        // If the tag is appended, the size field in footer/header contains the size of
        // all items and the footer, but not the initial header size.
        const int size = detail::parse_be<int>(&s[s.size() - 20]);
        if(size <= 0) { return -1; }
        return s.size() - size - header::size;
    }
    else
    {
        return -1;
    }
}

template<typename Source>
bool is_tagged(const Source& s)
{
    static_assert(detail::is_source<Source>::value, "Source requirements not met");
    return (s.size() >= header::size) && (find_tag_start(s) != -1);
}

template<typename Source>
tag parse(const Source& s)
{
    static_assert(detail::is_source<Source>::value, "Source requirements not met");

    if(s.size() < header::size) { return {}; }

    const int tag_start = find_tag_start(s);
    if(tag_start == -1) { return {}; }

    tag tag;
    const auto header = parse_header(&s[tag_start]);
    if(!is_header_valid(header)) { return {}; }

    tag.version = header.version;
    tag.items.reserve(header.num_items);
    for(auto i = 0, offset = tag_start + header::size;
        (offset < s.size()) && (i < header.num_items);
        ++ i)
    {
        const auto item_begin = &s[offset];
        const auto item_header = parse_item_header(item_begin);
        // A 0x00 byte separates the item key from its value.
        const auto key_begin = item_begin + 8;
        const auto sep_pos = std::find(key_begin, &s[s.size()], 0); 
        if(sep_pos == &s[s.size()]) { break; }

        const auto value_begin = sep_pos + 1;
        const int key_length = sep_pos - key_begin;
        const int value_length = std::min(item_header.value_size,
            s.size() - offset + 8 + key_length + 1);

        const auto key = item_key_from_string(key_begin, key_length);
        if(key != -1)
            tag.items.emplace_back(tag::item{std::string(key_begin, key_length),
                std::string(value_begin, value_length)});
        
        offset += 8 + key_length + 1 + value_length;
    }
    return tag;
}

template<typename Source>
simple_tag simple_parse(const Source& s)
{
    static_assert(detail::is_source<Source>::value, "Source requirements not met");

    if(s.size() < header::size) { return {}; }

    const int tag_start = find_tag_start(s);
    if(tag_start == -1) { return {}; }

    simple_tag tag;
    const auto header = parse_header(&s[tag_start]);
    if(!is_header_valid(header)) { return {}; }

    for(auto i = 0, offset = tag_start + header::size;
        (offset < s.size()) && (i < header.num_items);
        ++ i)
    {
        const auto item_begin = &s[offset];
        const auto item_header = parse_item_header(item_begin);
        // A 0x00 byte separates the item key from its value.
        const auto key_begin = item_begin + 8;
        const auto sep_pos = std::find(key_begin, &s[s.size()], 0); 
        if(sep_pos == &s[s.size()]) { break; }

        const auto value_begin = sep_pos + 1;
        const int key_length = sep_pos - key_begin;
        const int value_length = std::min(item_header.value_size,
            s.size() - offset + 8 + key_length + 1);
        switch(key_length) {
#define KEY_EQUALS(s) std::equal(key_begin, key_begin + sizeof(s) - 1, s)
        case 4:
            if(KEY_EQUALS("YEAR"))
                tag.year = std::atoi(value_begin);
            break;
        case 5:
            if(KEY_EQUALS("ALBUM"))
                tag.album = std::string(value_begin,  value_length);
            else if(KEY_EQUALS("TITLE"))
                tag.title = std::string(value_begin,  value_length);
            else if(KEY_EQUALS("GENRE"))
                tag.genre/* = TODO*/;
            else if(KEY_EQUALS("TRACK"))
                // Track number may be a single integer of track/total, but since
                // simple tag only has a track number field, we ignore the rest.
                tag.track_number = std::atoi(value_begin);
            break;
        case 6:
            if(KEY_EQUALS("ARTIST"))
                // TODO FIXME values may be a list instead of a string, which means that
                // entries are separated by 0x00 bytes. this is used if multiple artists
                // worked on the song
                tag.artist = std::string(value_begin, value_length);
            break;
        case 8:
            // TODO is this correct?
            if(!tag.artist.empty() && KEY_EQUALS("COMPOSER"))
                tag.artist = std::string(value_begin, value_length);
            break;
        case 9:
            // TODO is this correct?
            if(!tag.artist.empty() && KEY_EQUALS("CONDUCTOR"))
                tag.artist = std::string(value_begin, value_length);
            break;
#undef KEY_EQUALS
        }
        offset += 8 + key_length + 1 + value_length;
    }
    return tag;
}

constexpr static const struct {
    int key;
    const char* raw;
    const char* pretty;
} item_keys[] = {
    {tag::item::abstract, "ABSTRACT", "abstract"},
    {tag::item::album, "ALBUM", "album"},
    {tag::item::artist, "ARTIST", "artist"},
    {tag::item::bibliography, "BIBLIOGRAPHY", "bibliography"},
    {tag::item::catalog_number, "CATALOG", "catalog number"},
    {tag::item::comment, "COMMENT", "comment"},
    {tag::item::composer, "COMPOSER", "composer"},
    {tag::item::conductor, "CONDUCTOR", "conductor"},
    {tag::item::copyright, "COPYRIGHT", "copyright"},
    {tag::item::debut_album, "DEBUT ALBUM", "debut album"},
    {tag::item::dummy, "DUMMY", "dummy"},
    {tag::item::ean_upc, "EAN/UPC", "EAN/UPC"},
    {tag::item::file_location, "FILE", "file location"},
    {tag::item::genre, "GENRE", "genre"},
    {tag::item::index, "INDEX", "index"},
    {tag::item::introplay, "INTROPLAY", "introplay"},
    {tag::item::isbn, "ISBN", "isbn"},
    {tag::item::isrc, "ISRC", "isrc"},
    {tag::item::label_code, "LC", "label code"},
    {tag::item::language, "LANGUAGE", "language"},
    {tag::item::media, "MEDIA", "media"},
    {tag::item::publication_right_holder, "PUBLICATION RIGHT HOLDER", "publication right holder"},
    {tag::item::publisher, "PUBLISHER", "publisher"},
    {tag::item::record_date, "RECORD DATE", "record date"},
    {tag::item::record_location, "RECORD LOCATION", "record location"},
    {tag::item::related, "RELATED", "related"},
    {tag::item::subtitle, "SUBTITLE", "subtitle"},
    {tag::item::title, "TITLE", "title"},
    {tag::item::track, "TRACK", "track"},
    {tag::item::year, "YEAR", "year"},
    {tag::item::year, nullptr, "unknown"}
};

inline int item_key_from_string(const std::string& s) noexcept
{
    /*
    const auto it = std::find_if(std::begin(item_keys), std::end(item_keys),
        [&s](const auto& f) { return std::equal(f.raw, f.raw + 4, &s[0]); });
    if(it != std::end(item_keys))
        return it->key;
    else
        return -1;
    */
}

inline int item_key_from_string(const char* s, const int s_length) noexcept
{
    /*
    const auto it = std::find_if(std::begin(item_keys), std::end(item_keys),
        [s, s_length](const auto& f)
        {
            // TODO how to compare variable length const char* s without segfaulting?
            return false;
        });
    if(it != std::end(item_keys))
        return it->key;
    else
        return -1;
    */
}

constexpr const char* item_key_to_string(const int key) noexcept
{
    if((key >= int(tag::item::abstract)) && (key <= int(tag::item::year)))
        return item_keys[key].raw; 
    else
        return nullptr;
}

constexpr const char* item_key_to_hrstring(const int key) noexcept
{
    if((key >= int(tag::item::abstract)) && (key <= int(tag::item::year)))
        return item_keys[key].pretty; 
    else
        return nullptr;
}

} // namespace ape
} // namespace atag

#endif // ATAG_APE_IMPL
