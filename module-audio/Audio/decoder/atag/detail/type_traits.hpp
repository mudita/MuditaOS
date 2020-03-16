#ifndef ATAG_TYPE_TRAITS_HEADER
#define ATAG_TYPE_TRAITS_HEADER

#include <type_traits>

namespace atag
{
    namespace detail
    {

        template <typename... Ts> struct make_void
        {
            using type = void;
        };

        template <typename... Ts> using void_t = typename make_void<Ts...>::type;

        template <typename T, typename = void> struct is_source : std::false_type
        {};

        template <typename T>
        struct is_source<T, void_t<decltype(std::declval<T &>().operator[]()), decltype(std::declval<T &>().size())>>
            : std::true_type
        {};

    } // namespace detail
} // namespace atag

#endif // ATAG_TYPE_TRAITS_HEADER
