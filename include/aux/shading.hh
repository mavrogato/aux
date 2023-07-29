#ifndef INCLUDE_AUX_SHADING_HH_26D6013F_5344_4213_919E_FE4D10AB988F
#define INCLUDE_AUX_SHADING_HH_26D6013F_5344_4213_919E_FE4D10AB988F

#include <cstdint>

#include "aux/algebra.hh"

namespace aux::inline shading
{
    using color = aux::versor<uint8_t, 4>;
    inline uint8_t  a(color  c) noexcept { return c[3]; }
    inline uint8_t& a(color& c) noexcept { return c[3]; }
    inline uint8_t  r(color  c) noexcept { return c[2]; }
    inline uint8_t& r(color& c) noexcept { return c[2]; }
    inline uint8_t  g(color  c) noexcept { return c[1]; }
    inline uint8_t& g(color& c) noexcept { return c[1]; }
    inline uint8_t  b(color  c) noexcept { return c[0]; }
    inline uint8_t& b(color& c) noexcept { return c[0]; }

    template <uint8_t A = 0xff>
    constexpr color calc_hue(uint32_t h) noexcept {
        constexpr uint8_t S = 0xff;
        if (6*S <= h) h %= (S*6);
        uint8_t v = static_cast<uint8_t>(h);
        if (h <= S) return { 0, v, S, A };
        h -= S;
        v -= S;
        if (h <= S) return { 0, S,~v, A };
        h -= S;
        v -= S;
        if (h <= S) return { v, S, 0, A };
        h -= S;
        v -= S;
        if (h <= S) return { S,~v, 0, A };
        h -= S;
        v -= S;
        if (h <= S) return { S, 0, v, A };
        h -= S;
        v -= S;
        return {~v, 0, S, A };
    }

    template <uint8_t A = 0xff>
    constexpr auto hue_table = []{
        std::array<color, 1530> table;
        for (uint32_t i = 0; i < table.size(); ++i) {
            table[i] = calc_hue<A>(i);
        }
        return table;
    }();

    template <uint8_t A = 0xff>
    constexpr color hue(uint32_t h) {
        return hue_table<A>[h % 1530];
    }

} // ::aux::shading

#endif/*INCLUDE_AUX_SHADING_HH_26D6013F_5344_4213_919E_FE4D10AB988F*/
