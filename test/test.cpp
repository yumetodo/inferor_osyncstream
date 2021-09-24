#ifndef IUTEST_USE_MAIN
#define IUTEST_USE_MAIN
#endif
#include <inferior/osyncstream.hpp>
#include <iutest.hpp>
#include <sstream>

#include "constant.hpp"
template<typename T>
struct Basic : public ::iutest::Test {};
IUTEST_TYPED_TEST_CASE(Basic, ::iutest::Types<char, wchar_t>);
IUTEST_TYPED_TEST(Basic, InnerOuter) {
    std::basic_ostringstream<TypeParam> outer{};
    {
        inferior::basic_osyncstream<TypeParam> inner{outer};
        inner << constant::hello_world<TypeParam>();
        inner << std::endl;
        IUTEST_ASSERT(outer.str().empty());
    }
    outer << constant::arikitari_na_world_lf<TypeParam>();
    IUTEST_ASSERT_EQ(constant::inner_outer_expected<TypeParam>(), outer.str());
}
IUTEST_TYPED_TEST(Basic, ClearForReuseFeature) {
    std::basic_ostringstream<TypeParam> outer{};
    {
        inferior::basic_osyncstream<TypeParam> inner{outer};
        inner << constant::hello_world_lf<TypeParam>();
        IUTEST_ASSERT(outer.str().empty());
        inner.emit();
        IUTEST_ASSERT_EQ(constant::clear_for_reuse_feature_expected_1<TypeParam>(), outer.str());
        inner << constant::arikitari_na_sekai_lf<TypeParam>()
              << std::flush;  // did flush\nneedsflush\did flush\n
    }
    outer << constant::arikitari_na_world_lf<TypeParam>();
    IUTEST_ASSERT_EQ(constant::clear_for_reuse_feature_expected_2<TypeParam>(), outer.str());
}
