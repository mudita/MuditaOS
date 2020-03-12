#ifndef ATAG_FLAC_HEADER
#define ATAG_FLAC_HEADER

#include <string>

namespace atag
{
    namespace flac
    {

        struct tag
        {
            std::string title;
            std::string album;
            std::string artist;
            std::string genre;
            int year;
            int track_number;
            int sample_rate; // in Hz
            int num_channels;
            int num_samples;
        };

        /** Tests whether `s` contains a FLAC tag. */
        template <typename Source> bool is_tagged(const Source &s) noexcept;

        /** Parses and extracts all frames found in `s`. */
        template <typename Source> tag parse(const Source &s);

    } // namespace flac
} // namespace atag

#include "impl/flac.ipp"

#endif // ATAG_FLAC_HEADER
