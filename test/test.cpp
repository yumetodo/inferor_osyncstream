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
IUTEST_TYPED_TEST(Basic, NestedBufferedStream) {
    std::basic_ostringstream<TypeParam> outs{};
    {
        inferior::basic_osyncstream<TypeParam> outer{outs};
        outer << constant::arikitari<TypeParam>();
        {
            inferior::basic_osyncstream<TypeParam> inner{outer.get_wrapped()};
            inner << constant::hello_world_lf<TypeParam>();
            IUTEST_ASSERT(outs.str().empty());
            inner.emit();
            inner << constant::arikitari_na_sekai_lf<TypeParam>() << std::flush;
        }
        outer << constant::na_world_lf<TypeParam>();
    }
    IUTEST_ASSERT_EQ(constant::clear_for_reuse_feature_expected_2<TypeParam>(), outs.str());
}
IUTEST_TYPED_TEST(Basic, OStreamWithSharingStreambuf) {
    std::basic_ostringstream<TypeParam> out{};
    std::basic_ostream<TypeParam> out2{out.rdbuf()};
    using osyncstream = inferior::basic_osyncstream<TypeParam>;
    {
        osyncstream inner{out};
        osyncstream inner2{out2};
        inner << constant::inner<TypeParam>() << std::endl;
        inner2 << constant::inner2_lf<TypeParam>();
    }
    IUTEST_ASSERT_EQ(constant::ostream_with_sharing_streambuf_expected<TypeParam>(), out.str());
}
IUTEST_TYPED_TEST(Basic, MoveCtor) {
    auto& locks = inferior::detail::streambuf_locks::init();
    auto const sz = locks.size();
    using osyncstream = inferior::basic_osyncstream<TypeParam>;
    std::basic_ostringstream<TypeParam> out{};
    {
        osyncstream os{out};
        os << constant::arikitari<TypeParam>();
        {
            osyncstream os1{std::move(os)};
            os1 << constant::na_world_lf<TypeParam>();
            IUTEST_ASSERT_EQ(sz + 1, locks.size());
        }
        IUTEST_ASSERT_EQ(constant::arikitari_na_world_lf<TypeParam>(), out.str());
        IUTEST_ASSERT_EQ(sz, locks.size());
    }
    IUTEST_ASSERT_EQ(sz, locks.size());
}
