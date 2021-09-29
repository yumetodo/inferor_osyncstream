#ifndef INFERIOR_OSTNCSTREAM_HPP_
#define INFERIOR_OSTNCSTREAM_HPP_
#include <iosfwd>
#include <ostream>
#include <streambuf>
#include <string>

#include "streambuf_locks.hpp"

namespace inferior {
template<class charT, class traits = std::char_traits<charT>>
class basic_syncbuf : public std::basic_streambuf<charT, traits> {
public:
    using char_type = charT;
    using int_type = typename traits::int_type;
    using pos_type = typename traits::pos_type;
    using off_type = typename traits::off_type;
    using traits_type = traits;
    using streambuf_type = std::basic_streambuf<charT, traits>;

private:
    using string_type = std::basic_string<charT, traits>;
    using size_type = typename string_type::size_type;

public:
    explicit basic_syncbuf(streambuf_type* obuf = nullptr)
        : wrapped(obuf), buffer(), lock(detail::streambuf_locks::get(obuf)) {}
    basic_syncbuf(basic_syncbuf&&) = default;
    ~basic_syncbuf() noexcept {
        try {
            this->emit();
        } catch (...) {
            //
        }
    }
    basic_syncbuf& operator=(basic_syncbuf&&) = default;
    bool emit() {
        struct scope_exit {
            string_type& buffer;
            bool& needs_flush;
            scope_exit(string_type& buffer, bool& needs_flush)
                : buffer(buffer), needs_flush(needs_flush) {}
            ~scope_exit() {
                this->buffer.clear();
                this->needs_flush = false;
            }
        } ensure(this->buffer, this->needs_flush);
        if (!this->wrapped || !lock) return false;
        std::lock_guard<std::mutex> lk{lock};
        const auto re = this->wrapped->sputn(this->buffer.c_str(), this->buffer.size());
        if (0 < re && std::size_t(re) != this->buffer.size()) return false;
        if (this->needs_flush) {
            if (this->wrapped->pubsync() == -1) return false;
        }
        return true;
    }
    streambuf_type* get_wrapped() const noexcept { return wrapped; }
    void set_emit_on_sync(bool b) noexcept { this->emit_on_sync = b; }

protected:
    int sync() override {
        this->needs_flush = true;
        return this->emit_on_sync ? this->emit() ? 0 : traits_type::eof() : 0;
    }
    std::streamsize xsputn(const char_type* s, std::streamsize n) override {
        if (n < 0) return 0;
        this->buffer.append(s, n);
        return n;
    }
    int_type overflow(int_type c = traits_type::eof()) override {
        if (c != traits_type::eof()) this->buffer.push_back(traits_type::to_char_type(c));
        return c;
    }
    virtual pos_type seekoff(
        off_type /*off*/,
        std::ios_base::seekdir /*dir*/,
        std::ios_base::openmode /*which*/ = std::ios_base::in | std::ios_base::out) override {
        return pos_type(-1);  // always fail
    }
    virtual pos_type seekpos(
        pos_type /*pos*/,
        std::ios_base::openmode /*which*/ = std::ios_base::in | std::ios_base::out) override {
        // return this->seekoff(off_type(pos), std::ios_base::beg, which);
        return pos_type(-1);  // always fail
    }

private:
    streambuf_type* wrapped;
    bool emit_on_sync{};
    bool needs_flush{};
    string_type buffer;
    detail::streambuf_lock_proxy lock;
};

using syncbuf = basic_syncbuf<char>;
using wsyncbuf = basic_syncbuf<wchar_t>;

template<class charT, class traits = std::char_traits<charT>>
class basic_osyncstream : public std::basic_ostream<charT, traits> {
public:
    using char_type = charT;
    using int_type = typename traits::int_type;
    using pos_type = typename traits::pos_type;
    using off_type = typename traits::off_type;
    using traits_type = traits;
    using streambuf_type = std::basic_streambuf<charT, traits>;
    using syncbuf_type = basic_syncbuf<charT, traits>;

private:
    using base = std::basic_ostream<charT, traits>;

public:
    explicit basic_osyncstream(streambuf_type* obuf) : base{nullptr}, sb(obuf) { this->init(&sb); }
    explicit basic_osyncstream(std::basic_ostream<charT, traits>& os)
        : basic_osyncstream(os.rdbuf()) {}
    basic_osyncstream(basic_osyncstream&& o) noexcept : base(std::move(o)), sb(std::move(o.sb)) {}
    ~basic_osyncstream() = default;
    basic_osyncstream& operator=(basic_osyncstream&&) = default;
    void emit() { this->sb.emit(); }
    streambuf_type* get_wrapped() const noexcept { return this->sb.get_wrapped(); }
    syncbuf_type* rdbuf() const noexcept { return &sb; }

private:
    syncbuf_type sb;  // exposition on
};

using osyncstream = basic_osyncstream<char>;
using wosyncstream = basic_osyncstream<wchar_t>;
}  // namespace inferior
#endif  // INFERIOR_OSTNCSTREAM_HPP_
