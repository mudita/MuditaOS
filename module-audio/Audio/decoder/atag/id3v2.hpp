#ifndef ATAG_ID3V2_HEADER
#define ATAG_ID3V2_HEADER

#include "simple_tag.hpp"
#include "genres.hpp"

#include <vector>
#include <array>
#include <string>
#include <initializer_list>

namespace atag {
namespace id3v2 {

/** Each text frame includes a single byte to denote the encoding of its data. */
enum encoding
{
    iso_8859_1 = 0,
    utf16 = 1,
    utf16be = 2,
    utf8 = 3,
};

/**
 * This is a lower level representation of the tag, however, it does not directly
 * correspond to how the tag is represented in memory (i.e. the raw data). For instance,
 * the extended header is not included, nor the footer, if one is present (as it's
 * mostly just a copy of the header, i.e. holds no relevant information).
 */
struct tag
{
    struct frame
    {
        /**
         * So as to remain unambiguous with the official ID3 documentation, the original 
         * cryptic frame names have been preserved. To see what an individual frame
         * means, see: http://id3.org/id3v2.4.0-frames or use id3v2::hrid, which maps
         * a more readable alternative to each entry here.
         */
        enum id
        {
            aenc, apic, aspi, comm, comr, encr, equa, etco, geob, grid, ipls, link, mcdi,
            mllt, owne, pcnt, popm, poss, priv, rbuf, rvad, rvrb, seek, sign, sylt, sytc,

            // -- text information frames --
            talb, tbpm, tcom, tcon, tcop, tdat, tden, tdly, tenc, text, tflt, time, tit1,
            tit2, tit3, tkey, tlan, tlen, tmed, toal, tofn, toly, tope, tory, town, tpe1,
            tpe2, tpe3, tpe4, tpos, tpub, trck, trda, trsn, trso, tsiz, tsrc, tsse, txxx,
            tyer,

            ufid, user, uslt,

            // -- URL link frames --
            wcom, wcop, woaf, woar, woas, wors, wpay, wpub, wxxx,
        };

        enum flags : uint16_t
        {
            // -- frame status flags --
            tag_preservation  = 1 << 14,
            file_preservation = 1 << 13,
            read_only         = 1 << 12,
            // -- frame format flags --
            grouping_identity = 1 << 6,
            compression       = 1 << 3,
            encryption        = 1 << 2,
            unsynchronisation = 1 << 1,
            length_indicator  = 1 << 0,
        };

        std::string data;
        uint16_t flags;
        // Identifies what type of frame this is.
        uint8_t id;
        // The encoding of data if it's a text, irrelevant otherwise.
        uint8_t encoding;
    };

    std::vector<frame> frames;

    enum flags : uint8_t
    {
        has_footer        = 1 << 4,
        experimental      = 1 << 5,
        extended          = 1 << 6,
        unsynchronisation = 1 << 7,
    };

    uint8_t version;
    uint8_t revision;
    uint8_t flags;
};

inline bool is_text_frame(const int id) noexcept
{
    return (id >= tag::frame::talb) && (id <= tag::frame::tyer);
}

inline bool is_url_frame(const int id) noexcept
{
    return (id >= tag::frame::wcom) && (id <= tag::frame::wxxx);
}

/**
 * Returns -1 if s does not contain a valid frame id.
 *
 * `s` has to be at least 4 bytes long.
 */
template<typename String>
int frame_id_from_string(const String& s) noexcept;

/** Both return a nullptr if `id` is not a valid frame id. */
constexpr const char* frame_id_to_string(const int id) noexcept;
constexpr const char* frame_id_to_hrstring(const int id) noexcept;

/** Tests whether `s` contains an ID3v2 tag, which may be prepended or appended. */
template<typename Source>
bool is_tagged(const Source& s) noexcept;

/**
 * Since an ID3v2 tag can contain information beyond what is usually needed, this method 
 * only specifically parses the most important fields to identify music (title, album, 
 * artist etc). It is thus slightly faster than the regular parse method.
 *
 * Note that if textual frames are not UTF-8 encoded, they are converted to UTF-8 for
 * disambiguation, while "raw frames" produces by all parse overloads, keep their
 * original encoding, so these have to be converted by the user.
 */
template<typename Source>
simple_tag simple_parse(const Source& s);

/** Parses and extracts all frames found in s. */
template<typename Source>
tag parse(const Source& s);

/**
 * This overload provides a way to parse only the frames specified in wanted_frames.
 *
 * Example:
 * ```
 * using namespace atag;
 * id3v2::tag tag = id3v2::parse(source, {id3v2::composer, id3v2::album,
 *     id3v2::title, id3v2::year, id3v2::track_number});
 * for(const auto& frame : tag.frames) {
 *     // Do something with frame.
 * }
 * ```
 */
template<typename Source>
tag parse(const Source& s, const std::initializer_list<int>& wanted_frames);

/**
 * This overload expects a user defined predicate which takes a single int or a
 * `tag::frame::id` argument and returns true if the frame satisfies the user's criteria.
 *
 * Example:
 * ```
 * using namespace atag;
 * // Parse all text information frames.
 * auto tag1 = id3v2::parse(source1, id3v2::is_text_frame);
 * // Or provide a custom predicate.
 * auto tag2 = id3v2::parse(source2, [](const auto frame_id) { // ...  });
 * ```
 */
template<typename Source, typename Predicate>
tag parse(const Source& s, Predicate pred);

enum hrid
{
    audio_encryption = tag::frame::aenc,
    attached_picture = tag::frame::apic,
    audio_seek_point_index = tag::frame::aspi,
    comments = tag::frame::comm,
    commercial_frame = tag::frame::comr,
    encryption_method_registration = tag::frame::encr,
    equalisation = tag::frame::equa,
    event_timing_codes = tag::frame::etco,
    general_encapsulated_object = tag::frame::geob,
    group_identification_registration = tag::frame::grid,
    involved_people_list = tag::frame::ipls,
    linked_information = tag::frame::link,
    music_cd_id = tag::frame::mcdi,
    mpeg_location_lookup_table = tag::frame::mllt,
    ownership_frame = tag::frame::owne,
    play_counter = tag::frame::pcnt,
    popularimeter = tag::frame::popm,
    position_sync_frame = tag::frame::poss,
    private_frame = tag::frame::priv,
    recommended_buffer_size = tag::frame::rbuf,
    relative_vol_adjustment = tag::frame::rvad,
    reverb = tag::frame::rvrb,
    seek_frame = tag::frame::seek,
    signature_frame = tag::frame::sign,
    synced_lyryc = tag::frame::sylt,
    synced_tempo_codes = tag::frame::sytc,

    album = tag::frame::talb,
    beats_per_min = tag::frame::tbpm,
    composer = tag::frame::tcom,
    content_type = tag::frame::tcon,
    copyright = tag::frame::tcop,
    date = tag::frame::tdat,
    encoding_time = tag::frame::tden,
    playlist_delay = tag::frame::tdly,
    encoded_by = tag::frame::tenc,
    lyricist = tag::frame::text,
    file_type = tag::frame::tflt,
    time = tag::frame::time,
    content_group = tag::frame::tit1,
    title = tag::frame::tit2,
    subtitle = tag::frame::tit3,
    initial_key = tag::frame::tkey,
    language = tag::frame::tlan,
    length = tag::frame::tlen,
    media_type = tag::frame::tmed,
    original_title = tag::frame::toal,
    original_filename = tag::frame::tofn,
    original_lyricist = tag::frame::toly,
    original_performer = tag::frame::tope,
    original_release_year = tag::frame::tory,
    licensee = tag::frame::town,
    lead_artist = tag::frame::tpe1,
    accompaniment = tag::frame::tpe2,
    conductor = tag::frame::tpe3,
    remixed_by = tag::frame::tpe4,
    part_of_a_set = tag::frame::tpos,
    publisher = tag::frame::tpub,
    track_number = tag::frame::trck,
    recording_dates = tag::frame::trda,
    inet_radio_station_name = tag::frame::trsn,
    inet_radio_station_owner = tag::frame::trso,
    size = tag::frame::tsiz,
    isrc = tag::frame::tsrc,
    encoding_settings = tag::frame::tsse,
    user_defined_text = tag::frame::txxx,
    year = tag::frame::tyer,

    unique_file_id = tag::frame::ufid,
    terms_of_use = tag::frame::user,
    unsynced_lyrics = tag::frame::uslt,

    commercial_info = tag::frame::wcom,
    copyright_info = tag::frame::wcop,
    audio_file_webpage = tag::frame::woaf,
    artist_webpage = tag::frame::woar,
    audio_source_webpage = tag::frame::woas,
    internet_radio_station_webpage = tag::frame::wors,
    payment = tag::frame::wpay,
    publisher_webpage = tag::frame::wpub,
    user_defined_link = tag::frame::wxxx,
};

} // namespace id3v2
} // namespace atag

#include "impl/id3v2.ipp"

#endif // ATAG_ID3V2_HEADER
