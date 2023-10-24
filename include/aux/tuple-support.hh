#ifndef INCLUDE_AUX_TUPLE_HH_9FC3968B_8F14_4FA6_B497_C23EF1E23A5C
#define INCLUDE_AUX_TUPLE_HH_9FC3968B_8F14_4FA6_B497_C23EF1E23A5C

#include <concepts>
#include <iosfwd>
#include <tuple>

namespace aux::inline tuple
{
    template <class First, class... Rest>
    class packed_tuple {
    public:
        packed_tuple(packed_tuple const&) = default;
        packed_tuple(packed_tuple&&) = default;
        packed_tuple& operator=(packed_tuple const&) = default;
        packed_tuple& operator=(packed_tuple&&) = default;
        auto operator<=>(packed_tuple const&) const = default;

        constexpr packed_tuple(First first = First(), Rest... rest)
            : first{first}
            , rest{rest...}
            {
            }

    public:
        template <size_t N>
        constexpr auto const& get() const {
            if constexpr (N == 0) {
                return this->first;
            }
            else {
                return rest.template get<N-1>();
            }
        }
        template <size_t N>
        constexpr auto& get() {
            if constexpr (N == 0) {
                return this->first;
            }
            else {
                return rest.template get<N-1>();
            }
        }

    private:
        First first;
        packed_tuple<Rest...> rest;
    } __attribute__((__packed__));

    template <class First>
    class packed_tuple<First> {
    public:
        packed_tuple(packed_tuple const&) = default;
        packed_tuple(packed_tuple&&) = default;
        packed_tuple& operator=(packed_tuple const&) = default;
        packed_tuple& operator=(packed_tuple&&) = default;
        auto operator<=>(packed_tuple const&) const = default;

    public:
        packed_tuple(First first = First())
            : first(first)
            {
            }

        template <size_t N> auto const& get() const { static_assert(N == 0); return this->first; }
        template <size_t N> auto&       get()       { static_assert(N == 0); return this->first; }

    private:
        First first;
    } __attribute__((__packed__));

    template <size_t I, class... Args>
    auto& get(packed_tuple<Args...>& t) noexcept {
        return t.template get<I>();
    }
    template <size_t I, class... Args>
    auto const& get(packed_tuple<Args...> const& t) noexcept {
        return t.template get<I>();
    }

    template <class T, size_t I>
    concept has_tuple_element = requires(T t) {
        typename std::tuple_element_t<I, std::remove_const_t<T>>;
        { get<I>(t) } -> std::convertible_to<std::tuple_element_t<I, T> const&>;
    };
    template <class T>
    concept tuple_like = !std::is_reference_v<T> && requires(T) {
        std::tuple_size<T>::value;
        //requires std::derived_from<std::tuple_size<T>, std::integral_constant<size_t, std::tuple_size_v<T>>>;
    } && []<size_t... I>(std::index_sequence<I...>) noexcept {
        return (has_tuple_element<T, I>&& ...);
    }(std::make_index_sequence<std::tuple_size_v<T>>());
 
} // ::aux::tuple

namespace std
{
    template <class... Args>
    struct tuple_size<aux::tuple::packed_tuple<Args...>> { static constexpr auto value = sizeof... (Args); };
    template <class... Args>
    constexpr size_t tuple_size_v<aux::tuple::packed_tuple<Args...>> = sizeof... (Args);
    template <size_t I, class... Args>
    struct tuple_element<I, aux::tuple::packed_tuple<Args...>> {
        using type = decltype(std::declval<aux::tuple::packed_tuple<Args...>>().template get<I>());
    };

    template <class Ch, aux::tuple::tuple_like T>
    auto& operator<<(std::basic_ostream<Ch>& output, T const& t) noexcept {
        output.put('(');
        [&]<size_t ...I>(std::index_sequence<I...>) noexcept {
            (void) (int[]) {(output << (I==0 ? "" : " ") << get<I>(t), 0)...};
        }(std::make_index_sequence<std::tuple_size_v<T>>());
        return output.put(')');
    }
} // ::std

#endif/*INCLUDE_AUX_TUPLE_HH_9FC3968B_8F14_4FA6_B497_C23EF1E23A5C*/
