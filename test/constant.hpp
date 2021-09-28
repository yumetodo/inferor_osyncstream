#ifndef INFERIOR_OSYNCSTREAM_TEST_CONSTANT_HPP_
#define INFERIOR_OSYNCSTREAM_TEST_CONSTANT_HPP_
namespace constant {
#define INFERIOR_OSYNCSTREAM_TEST_CONSTANT_REGISTER_CHAR(identifier, no_prefix_char_literal) \
    template<typename CharType>                                                              \
    inline constexpr CharType identifier();                                                  \
    template<>                                                                               \
    inline constexpr char identifier<char>() {                                               \
        return no_prefix_char_literal;                                                       \
    }                                                                                        \
    template<>                                                                               \
    inline constexpr wchar_t identifier<wchar_t>() {                                         \
        return L##no_prefix_char_literal;                                                    \
    }
#define INFERIOR_OSYNCSTREAM_TEST_CONSTANT_REGISTER_STRING(identifier, no_prefix_string_literal) \
    template<typename CharType>                                                                  \
    inline constexpr const CharType* identifier();                                               \
    template<>                                                                                   \
    inline constexpr const char* identifier<char>() {                                            \
        return no_prefix_string_literal;                                                         \
    }                                                                                            \
    template<>                                                                                   \
    inline constexpr const wchar_t* identifier<wchar_t>() {                                      \
        return L##no_prefix_string_literal;                                                      \
    }                                                                                            \
    template<typename CharType>                                                                  \
    inline constexpr const CharType* identifier##_lf();                                          \
    template<>                                                                                   \
    inline constexpr const char* identifier##_lf<char>() {                                       \
        return no_prefix_string_literal "\n";                                                    \
    }                                                                                            \
    template<>                                                                                   \
    inline constexpr const wchar_t* identifier##_lf<wchar_t>() {                                 \
        return L##no_prefix_string_literal L"\n";                                                \
    }

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
INFERIOR_OSYNCSTREAM_TEST_CONSTANT_REGISTER_STRING(arikitari, "hello");
INFERIOR_OSYNCSTREAM_TEST_CONSTANT_REGISTER_STRING(na_world, "na_world");
INFERIOR_OSYNCSTREAM_TEST_CONSTANT_REGISTER_STRING(hello_world, "hello world");
INFERIOR_OSYNCSTREAM_TEST_CONSTANT_REGISTER_STRING(arikitari_na_world, "arikitari na world");
INFERIOR_OSYNCSTREAM_TEST_CONSTANT_REGISTER_STRING(arikitari_na_sekai, "arikitari na sekai");
INFERIOR_OSYNCSTREAM_TEST_CONSTANT_REGISTER_STRING(
    inner_outer_expected, "hello world\narikitari na world\n");
INFERIOR_OSYNCSTREAM_TEST_CONSTANT_REGISTER_STRING(
    clear_for_reuse_feature_expected_1, "hello world\n");
INFERIOR_OSYNCSTREAM_TEST_CONSTANT_REGISTER_STRING(
    clear_for_reuse_feature_expected_2, "hello world\narikitari na sekai\narikitari na world\n");
INFERIOR_OSYNCSTREAM_TEST_CONSTANT_REGISTER_STRING(inner, "inner");
INFERIOR_OSYNCSTREAM_TEST_CONSTANT_REGISTER_STRING(inner2, "inner2");
INFERIOR_OSYNCSTREAM_TEST_CONSTANT_REGISTER_STRING(ostream_with_sharing_streambuf_expected, "inner2\ninner\n");
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#undef INFERIOR_OSYNCSTREAM_TEST_CONSTANT_REGISTER_CHAR
#undef INFERIOR_OSYNCSTREAM_TEST_CONSTANT_REGISTER_STRING
}  // namespace constant
#endif
