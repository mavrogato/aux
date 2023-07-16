#ifndef INCLUDE_AUX_SUPPORT_LIKE_HH_9FC3968B_8F14_4FA6_B497_C23EF1E23A5C
#define INCLUDE_AUX_SUPPORT_LIKE_HH_9FC3968B_8F14_4FA6_B497_C23EF1E23A5C

#include <concepts>
#include <iosfwd>
#include <tuple>

namespace aux::tuple_support
{
    template <class T, size_t I>
    concept has_tuple_element = requires(T t) {
        typename std::tuple_element_t<I, std::remove_const_t<T>>;
        { get<I>(t) } -> std::convertible_to<std::tuple_element_t<I, T> const&>;
    };
    template <class T>
    concept tuple_like = !std::is_reference_v<T> && requires(T) {
        std::tuple_size<T>::value;
        //requires std::derived_from<std::tuple_size<T>, std::integral_constant<size_t, tuple_size_v<T>>>;
    } && []<size_t... I>(std::index_sequence<I...>) noexcept {
        return (has_tuple_element<T, I>&& ...);
    }(std::make_index_sequence<std::tuple_size_v<T>>());
 
} // ::aux::tuple_support

namespace std
{
    template <class Ch, aux::tuple_support::tuple_like T>
    auto& operator<<(std::basic_ostream<Ch>& output, T const& t) noexcept {
        output.put('(');
        [&]<size_t ...I>(std::index_sequence<I...>) noexcept {
            (void) (int[]) {(output << (I==0 ? "" : " ") << get<I>(t), 0)...};
        }(std::make_index_sequence<std::tuple_size_v<T>>());
        return output.put(')');
    }
} // ::std


#endif/*INCLUDE_AUX_TUPLE_SUPPORT_HH_9FC3968B_8F14_4FA6_B497_C23EF1E23A5C*/

