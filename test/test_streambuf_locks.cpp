#include <inferior/osyncstream.hpp>
#include <inferior/streambuf_locks.hpp>
#include <iutest.hpp>

using inferior::detail::streambuf_locks;

IUTEST(StreambufLocks, LockMapSimple) {
    const auto sz = streambuf_locks::init().size();
    auto& locks = streambuf_locks::init();
    std::ostringstream out{};
    {
        auto mx = streambuf_locks::get(out.rdbuf());
        {
            auto my = streambuf_locks::get(out.rdbuf());
            IUTEST_ASSERT_EQ(mx, my);
            locks.release_lock(my.unsafe_get_lock_ref(), out.rdbuf());
            IUTEST_ASSERT_EQ(1 + sz, locks.size());
        }
        IUTEST_ASSERT(mx);
        locks.release_lock(mx.unsafe_get_lock_ref(), out.rdbuf());
    }
    IUTEST_ASSERT_EQ(0 + sz, locks.size());
}

IUTEST(StreambufLocks, LockMapWithNullReturnsNull){

}
