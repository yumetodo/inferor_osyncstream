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

IUTEST_MAKE_PEEP(std::string inferior::syncbuf::*, inferior::syncbuf, buffer);
IUTEST_MAKE_PEEP(std::wstring inferior::wsyncbuf::*, inferior::wsyncbuf, buffer);
template<typename CharT>
std::basic_string<CharT> str(const inferior::basic_osyncstream<CharT>& ss) {
    const auto& buf = *ss.rdbuf();
    return IUTEST_PEEP_GET(buf, inferior::basic_syncbuf<CharT>, buffer);
}

IUTEST_TYPED_TEST(Basic, MoveAssignOp) {
    using osyncstream = inferior::basic_osyncstream<TypeParam>;
    using ostringstream = std::basic_ostringstream<TypeParam>;

    auto& locks = inferior::detail::streambuf_locks::init();
    auto const sz = locks.size();
    ostringstream out{};
    ostringstream dummy{};
    {
        osyncstream os{out};
        os << constant::hello_world_lf<TypeParam>();
        {
            osyncstream os1{dummy};
            os1 << constant::arikitari_lf<TypeParam>();
            IUTEST_ASSERT_EQ(constant::arikitari_lf<TypeParam>(), str(os1));
            IUTEST_ASSERT_EQ(sz + 2, locks.size());
            os1 = std::move(os);
            IUTEST_ASSERT_EQ(sz + 1, locks.size());
            IUTEST_ASSERT_EQ(constant::hello_world_lf<TypeParam>(), str(os1));
            os1 << constant::arikitari_na_world_lf<TypeParam>();
        }
        IUTEST_ASSERT_EQ(constant::arikitari_lf<TypeParam>(), dummy.str());
        IUTEST_ASSERT_EQ(constant::inner_outer_expected<TypeParam>(), out.str());
        IUTEST_ASSERT_EQ(sz, locks.size());
    }
    IUTEST_ASSERT_EQ(sz, locks.size());
}
IUTEST_TYPED_TEST(Basic, swap) {
    using osyncstream = inferior::basic_osyncstream<TypeParam>;
    using ostringstream = std::basic_ostringstream<TypeParam>;

    auto& locks = inferior::detail::streambuf_locks::init();
    auto const sz = locks.size();
    ostringstream out{};
    ostringstream dummy{};
    {
        osyncstream os{out};
        os << constant::hello_world_lf<TypeParam>();
        {
            osyncstream os1{dummy};
            os1 << constant::arikitari_lf<TypeParam>();
            IUTEST_ASSERT_EQ(constant::arikitari_lf<TypeParam>(), str(os1));
            using std::swap;
            swap(os1, os);
            IUTEST_ASSERT_EQ(constant::hello_world_lf<TypeParam>(), str(os1));
            os1 << constant::arikitari_na_world_lf<TypeParam>();
            IUTEST_ASSERT_EQ(sz + 2, locks.size());
        }
        os.emit();
        IUTEST_ASSERT_EQ(constant::arikitari_lf<TypeParam>(), dummy.str());
        IUTEST_ASSERT_EQ(constant::inner_outer_expected<TypeParam>(), out.str());
        IUTEST_ASSERT_EQ(sz + 1, locks.size());
    }
    IUTEST_ASSERT_EQ(sz, locks.size());
}
IUTEST_TYPED_TEST(Basic, manipulators) {
    using osyncstream = inferior::basic_osyncstream<TypeParam>;
    std::basic_ostringstream<TypeParam> outer{};
    {
        osyncstream inner{outer};
        inner << inferior::emit_on_flush;
        inner << constant::hello_world<TypeParam>();
        inner << std::endl;
        outer << constant::within<TypeParam>();
        inner << inferior::no_emit_on_flush;
        inner << constant::arikitari_na_world<TypeParam>() << std::endl;
        IUTEST_ASSERT_EQ(constant::manipulators_expected1<TypeParam>(), outer.str());
        inner << constant::arikitari_na_sekai_lf<TypeParam>() << inferior::flush_emit;
        IUTEST_ASSERT_EQ(constant::manipulators_expected2<TypeParam>(), outer.str());
    }
    outer << constant::fuji_lf<TypeParam>();
    IUTEST_ASSERT_EQ(constant::manipulators_expected3<TypeParam>(), outer.str());
}
IUTEST_TYPED_TEST(Basic, ManipulatorsForRegularOStream) {
    std::basic_ostringstream<TypeParam> outer{};
    outer << constant::hello_world<TypeParam>() << inferior::emit_on_flush << std::endl;
    IUTEST_ASSERT_EQ(constant::hello_world_lf<TypeParam>(), outer.str());
    outer << constant::arikitari_na_sekai<TypeParam>() << inferior::no_emit_on_flush << std::endl;
    IUTEST_ASSERT_EQ(constant::manipulators_expected4<TypeParam>(), outer.str());
    outer << constant::arikitari_na_world_lf<TypeParam>() << inferior::flush_emit;
    IUTEST_ASSERT_EQ(constant::clear_for_reuse_feature_expected_2<TypeParam>(), outer.str());
}
