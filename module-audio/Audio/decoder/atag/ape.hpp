#ifndef ATAG_APE_HEADER
#define ATAG_APE_HEADER

#include "simple_tag.hpp"

#include <vector>
#include <string>

namespace atag {
namespace ape {

struct tag
{
    struct item
    {
        enum key
        {
            abstract,
            album,
            artist,
            bibliography,
            catalog_number,
            comment,
            composer,
            conductor,
            copyright,
            debut_album,
            dummy,
            ean_upc,
            file_location,
            genre,
            index,
            introplay,
            isbn,
            isrc,
            label_code,
            language,
            media,
            publication_right_holder,
            publisher,
            record_date,
            record_location,
            related,
            subtitle,
            title,
            track,
            year,
        };

        int key;
        // A UTF-8 encoded string or binary data, depending on the item type.
        std::string data;
    };

    int version;
    std::vector<item> items;
};

/** Returns -1 if `s` does not contain a valid item key. */
int item_key_from_string(const std::string& s) noexcept;
int item_key_from_string(const char* s, const int s_length) noexcept;

/** Both return a nullptr if key is not a valid item key. */
constexpr const char* item_key_to_string(const int key) noexcept;
constexpr const char* item_key_to_hrstring(const int key) noexcept;

template<typename Source> bool is_tagged(const Source& s);

template<typename Source> simple_tag simple_parse(const Source& s);
template<typename Source> tag parse(const Source& s);

} // namespace ape
} // namespace atag

#endif // ATAG_APE_HEADER
