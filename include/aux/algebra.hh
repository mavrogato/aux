#ifndef INCLUDE_AUX_ALGEBRA_HH_EC13CFDA_BFC8_4730_B364_095C575E06B5
#define INCLUDE_AUX_ALGEBRA_HH_EC13CFDA_BFC8_4730_B364_095C575E06B5

#include <concepts>
#include <tuple>
#include <utility>
#include <initializer_list>
#include <type_traits>
#include <functional>
#include <bit>
#include <cstddef>

#include "aux/tuple-like.hh"

namespace aux::inline algebra
{
    template <class T, size_t N>
    struct versor : versor<T, N-1> {
    public:
        using base_type = versor<T, N-1>;
        using value_type = typename base_type::value_type;
        using iterator = typename base_type::iterator;
        using const_iterator = typename base_type::const_iterator;
        using reference = typename base_type::reference;
        using const_reference = typename base_type::const_reference;

    public:
        value_type last;

    public:
        constexpr versor(auto... args) noexcept
        : versor({static_cast<T>(args)...}, std::make_index_sequence<N-1>())
            {
                static_assert(sizeof... (args) <= N);
            }
    private:
        constexpr static auto place(std::initializer_list<T> const& args, size_t i) noexcept {
            return (i < args.size()) ? *(args.begin() + i) : T();
        }
        template <size_t ...I>
        constexpr versor(std::initializer_list<T>&& args, std::index_sequence<I...>) noexcept
            : base_type{place(args, I)...}, last{place(args, N-1)}
            {
            }

    public:
        template <class T2>
        versor<T2, N> coerce() const noexcept {
            return versor<T2, N>{base_type::template coerce<T2>(), static_cast<T2>(this->last)};
        }
        // (WIP) Constructor only used by coercing...
        constexpr versor(base_type base, value_type last) noexcept
            : base_type{base}, last{last}
            {
            }

    public:
        template <size_t I>
        constexpr auto get() const noexcept {
            static_assert(I < N);
            return static_cast<versor<T, I+1> const*>(this)->last;
        }
        template <size_t I>
        constexpr auto& get() noexcept {
            static_assert(I < N);
            return static_cast<versor<T, I+1>*>(this)->last;
        }
        template <size_t I>
        constexpr friend auto get(versor const& v) noexcept { return v.get<I>(); }
        template <size_t I>
        constexpr friend auto& get(versor& v) noexcept { return v.get<I>(); }

    public:
        constexpr auto size() const noexcept { return N; }

        constexpr auto begin() const noexcept { return &static_cast<versor<T, 1> const*>(this)->last; }
        constexpr auto begin()       noexcept { return &static_cast<versor<T, 1>      *>(this)->last; }
        constexpr auto end  () const noexcept { return &this->last + 1; }
        constexpr auto end  ()       noexcept { return &this->last + 1; }

        constexpr auto  front() const noexcept { return *(this->begin()); }
        constexpr auto& front()       noexcept { return *(this->begin()); }
        constexpr auto  back () const noexcept { return this->last; }
        constexpr auto& back ()       noexcept { return this->last; }

        constexpr auto  operator[](size_t i) const noexcept { return *(this->begin() + i); }
        constexpr auto& operator[](size_t i)       noexcept { return *(this->begin() + i); }

    public:
        // template <class Func, class ...Rest>
        // constexpr auto& apply(Func&& func, Rest&& ...rest) noexcept {
        //     this->last = func(this->last, rest.last...);
        //     if constexpr ((std::is_rvalue_reference_v<Rest> && ...)) {
        //         base_type::apply(std::forward<Func>(func), std::move(static_cast<base_type&&>(rest))...);
        //     }
        //     else {
        //         base_type::apply(std::forward<Func>(func), static_cast<base_type const&>(rest)...);
        //     }
        //     return *this;
        // }
        template <class Func>
        constexpr auto& apply(Func&& func) {
            this->last = func(this->last);
            base_type::apply(std::forward<Func>(func));
            return *this;
        }
        template <class Func>
        constexpr auto& apply(Func&& func, versor const& rhs) {
            this->last = func(this->last, rhs.last);
            base_type::apply(std::forward<Func>(func), static_cast<base_type const&>(rhs));
            return *this;
        }

    public:
        constexpr auto operator<=>(versor const& rhs) const noexcept = default;

        constexpr auto operator+() const noexcept { return *this; }

        constexpr auto& negate() noexcept { return apply(std::negate<T>()); }
        constexpr auto operator-() const noexcept { return (+(*this)).negate(); }
        constexpr auto& lognot() noexcept { return apply(std::bit_not<T>()); }
        constexpr auto operator~() const noexcept { return (+(*this)).lognot(); }

        constexpr auto& operator+=(auto&& rhs) noexcept { return apply(std::plus<T>(), rhs); }
        constexpr auto& operator-=(auto&& rhs) noexcept { return apply(std::minus<T>(), rhs); }
        constexpr auto& operator&=(auto&& rhs) noexcept { return apply(std::bit_and<T>(), rhs); }
        constexpr auto& operator|=(auto&& rhs) noexcept { return apply(std::bit_or<T>(), rhs); }
        constexpr auto& operator^=(auto&& rhs) noexcept { return apply(std::bit_xor<T>(), rhs); }

        constexpr auto operator+(auto&& rhs) const noexcept { return (+(*this)) += rhs; }
        constexpr auto operator-(auto&& rhs) const noexcept { return (+(*this)) -= rhs; }
        constexpr auto operator&(auto&& rhs) const noexcept { return (+(*this)) &= rhs; }
        constexpr auto operator|(auto&& rhs) const noexcept { return (+(*this)) |= rhs; }
        constexpr auto operator^(auto&& rhs) const noexcept { return (+(*this)) ^= rhs; }

        constexpr auto& operator*=(value_type s) noexcept {
            return apply([s](value_type x) noexcept {
                return x * s;
            });
        }
        constexpr auto operator*(value_type s) const noexcept { return (+(*this)) *= s; }
        constexpr friend auto operator*(value_type s, versor v) noexcept { return v * s; }

        constexpr auto& operator/=(value_type d) noexcept {
            return apply([d](value_type x) noexcept {
                return x / d;
            });
        }
        constexpr auto operator/(value_type d) const noexcept { return (+(*this)) /= d; }

        constexpr auto& operator%=(value_type d) noexcept {
            return apply([d](value_type x) noexcept {
                return x % d;
            });
        }
        constexpr auto operator%(value_type d) const noexcept { return (+(*this)) %= d; }

        constexpr auto& operator<<=(value_type s) noexcept {
            return apply([s](value_type x) noexcept {
                return x << s;
            });
        }
        constexpr auto operator<<(value_type s) const noexcept { return (+(*this)) <<= s; }

        constexpr auto& operator>>=(value_type s) noexcept {
            return apply([s](value_type x) noexcept {
                return x >> s;
            });
        }
        constexpr auto operator>>(value_type s) const noexcept { return (+(*this)) >>= s; }

        constexpr friend auto inner(versor const& a, versor const& b) noexcept {
            return a.last * b.last + inner(static_cast<base_type const&>(a), static_cast<base_type const&>(b));
        }
    };

    template <class T>
    struct versor<T, 0> {
    public:
        using value_type = T;
        using iterator = value_type*;
        using const_iterator = value_type const*;
        using reference = value_type&;
        using const_reference = value_type const&;
        using size_type = size_t;

        constexpr auto operator<=>(versor const& rhs) const noexcept = default;

    protected:
        template <class T2>
        constexpr versor<T2, 0> coerce() const noexcept { return {}; }
        constexpr auto& apply(auto&&...) noexcept { return *this; }

        constexpr friend T inner(versor const&, versor const&) noexcept { return T(); }
    };

    template <class T>
    constexpr auto cross(versor<T, 3> lhs, versor<T, 3> rhs) noexcept {
        return versor<T, 3>{
            lhs[1] * rhs[2] - lhs[2] * rhs[1],
            lhs[2] * rhs[0] - lhs[0] * rhs[2],
            lhs[0] * rhs[1] - lhs[1] * rhs[0],
        };
    }
    template <class T>
    constexpr auto det(versor<T, 3> a, versor<T, 3> b, versor<T, 3> c) noexcept {
        return inner(cross(a, b), c);
    }

    using vec2s = versor<short,  2>;
    using vec2i = versor<int,    2>;
    using vec2f = versor<float,  2>;
    using vec2d = versor<double, 2>;
    using vec3s = versor<short,  3>;
    using vec3i = versor<int,    3>;
    using vec3f = versor<float,  3>;
    using vec3d = versor<double, 3>;
    using vec4s = versor<short,  4>;
    using vec4i = versor<int,    4>;
    using vec4f = versor<float,  4>;
    using vec4d = versor<double, 4>;

} // ::aux::algebra

namespace std
{
    using namespace aux::algebra;

    template <class T, size_t N>
    struct tuple_size<versor<T, N>> { static constexpr auto value = N; };
    template <class T, size_t N>
    constexpr size_t tuple_size_v<versor<T, N>> = N;
    template <size_t I, class T, size_t N>
    struct tuple_element<I, versor<T, N>> { using type = T; };

} // ::std

#endif/*INCLUDE_AUX_ALGEBRA_HH_EC13CFDA_BFC8_4730_B364_095C575E06B5*/
