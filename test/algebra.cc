
#include <algorithm>
#include <complex>
#include <bit>

#include "aux/algebra.hh"

#include <gtest/gtest.h>


class algebra_test : public testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(algebra_test, versor_initializer) {
    aux::versor<double, 3> v3d;
    ASSERT_EQ(0, v3d[0]);
    ASSERT_EQ(0, v3d[1]);
    ASSERT_EQ(0, v3d[2]);

    aux::versor<float, 10> v10f;
    ASSERT_EQ(true, std::all_of(v10f.begin(), v10f.end(), [](float x) { return 0 == x; }));

    aux::versor<int, 4> v4i(1, 2, 3, 4);
    ASSERT_EQ(1, v4i[0]);
    ASSERT_EQ(2, v4i[1]);
    ASSERT_EQ(3, v4i[2]);
    ASSERT_EQ(4, v4i[3]);

    aux::versor<std::complex<double>, 2> v2c(std::complex<double>(1, 2),
                                             std::complex<double>(3, 4));
    ASSERT_EQ(1.0, v2c[0].real());
    ASSERT_EQ(2.0, v2c[0].imag());
    ASSERT_EQ(3.0, v2c[1].real());
    ASSERT_EQ(4.0, v2c[1].imag());

    ASSERT_EQ((aux::versor<double, 3>(1, 2, 3)),
              (aux::versor<double, 3>(1.0, 2.0, 3.0)));
}

TEST_F(algebra_test, versor_layout) {
    std::complex<double> z(1, 2);
    auto v = std::bit_cast<aux::versor<double, 2>>(z);
    ASSERT_EQ(v[0], z.real());
    ASSERT_EQ(v[1], z.imag());
}

TEST_F(algebra_test, linear) {
    constexpr aux::versor<double, 4> a{1,2,3,4};
    constexpr aux::versor<double, 4> b{9,8,7,6};
    constexpr aux::versor<double, 4> c = a + b;

    ASSERT_EQ(2*c - a - b, c);

    auto d = c - b;
    ASSERT_EQ(a, d);

    d *= 2;
    ASSERT_EQ(2*a, d);
    ASSERT_EQ(d/2, a);
    ASSERT_EQ(-a + d, a);
}

TEST_F(algebra_test, euclidean) {
    ASSERT_EQ(norm(std::versor<int, 2>{3, 4}), 25);
    EXPECT_EQ(abs(std::versor<int, 2>{3, 4}), 5);
}

TEST_F(algebra_test, bit_operation) {
    constexpr aux::versor<bool, 5> a;
    for (bool item : +a) ASSERT_EQ(item, false);
    for (bool item : ~a) ASSERT_EQ(item, true);

    for (bool item : a & (~a)) ASSERT_EQ(item, false);
    for (bool item : a | (~a)) ASSERT_EQ(item, true);

    ASSERT_EQ(a^a, (aux::versor<bool, 5>{}));
    ASSERT_EQ(a^~a, (~aux::versor<bool, 5>{}));

    constexpr auto b = (aux::versor<int, 2>{1,2} << 2);
    ASSERT_EQ(b, (aux::versor<int, 2>{4, 8}));
    ASSERT_EQ(b >> 3, (aux::versor<int, 2>{0, 1}));
}

TEST_F(algebra_test, vector_algebra) {
    constexpr aux::versor<double, 3> a{1,2,3};
    constexpr aux::versor<double, 3> b{4,5,6};
    constexpr aux::versor<double, 3> c{9,8,7};
    ASSERT_EQ(inner(a, b), 1*4 + 2*5 + 3*6);
    ASSERT_EQ(cross(a, cross(b, c)), b * inner(a, c) - c * inner(a, b));
    ASSERT_EQ(cross(aux::vec3d{1,0,0}, aux::vec3d{0,1,0}), (aux::vec3d{0,0,1}));
}

TEST_F(algebra_test, etc_operation) {
    auto a = aux::versor<double, 4>{1.1,2.2,3.3,4.4}.coerce<int>();
    ASSERT_EQ(a, (aux::versor<int, 4>{1,2,3,4}));
    ASSERT_EQ(sizeof (a), sizeof (int) * 4);
    ASSERT_EQ((aux::vec2i{3,4} % 2), (aux::vec2i{1,0}));
}
