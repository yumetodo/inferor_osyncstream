#include <inferior/osyncstream.hpp>
#include <inferior/streambuf_locks.hpp>
#include <iutest.hpp>
#include <sstream>
#include <thread>

#include "constant.hpp"
template<typename T>
struct Parallel : public ::iutest::Test {};
IUTEST_TYPED_TEST_CASE(Parallel, ::iutest::Types<char, wchar_t>);
IUTEST_TYPED_TEST(Parallel, ManyThreadsOnSingleStream) {
    auto get_streambuf_locks_size = [] { return inferior::detail::streambuf_locks::init().size(); };
    using osyncstream = inferior::basic_osyncstream<TypeParam>;
    const auto sz = get_streambuf_locks_size();
    std::basic_ostringstream<TypeParam> out1{}, out2{};
    {
        osyncstream outer2{out2};
        std::vector<std::thread> th{};
        for (int i = 0; i < 10; i++) {
            outer2 << "2 main" << i << "sz=" << get_streambuf_locks_size() << std::endl;
            th.push_back(std::thread([i, &out1, &out2] {
                osyncstream o2{out2};
                osyncstream{out1} << "hello " << i << std::endl;
                o2 << "2 hello " << i << std::endl;
            }));
            osyncstream{out1} << "main " << i << " sz=" << get_streambuf_locks_size() << std::endl;
        }
        for (auto& t : th) t.join();
    }
    IUTEST_ASSERT_EQ(sz, get_streambuf_locks_size());
}
