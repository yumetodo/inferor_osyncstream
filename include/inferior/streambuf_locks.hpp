#ifndef INFERIOR_STREAMBUF_LOCKS_HPP_
#define INFERIOR_STREAMBUF_LOCKS_HPP_
#include <memory>
#include <mutex>
#include <optional>
#include <unordered_map>
namespace inferior::detail {
class streambuf_locks;
class streambuf_lock_proxy {
public:
    streambuf_lock_proxy() = default;
    streambuf_lock_proxy(
        std::shared_ptr<std::mutex>&& lock, void* streambuf, streambuf_locks* locks) noexcept
        : lock(std::move(lock)), streambuf(streambuf), locks(locks) {}
    streambuf_lock_proxy(const streambuf_lock_proxy&) = delete;
    streambuf_lock_proxy(streambuf_lock_proxy&&) = default;
    streambuf_lock_proxy& operator=(const streambuf_lock_proxy&) = delete;
    streambuf_lock_proxy& operator=(streambuf_lock_proxy&&) = default;
    ~streambuf_lock_proxy() noexcept;

    bool operator!() const noexcept { return !this->lock; }
    explicit operator bool() const noexcept { return !(*this); }
    operator std::mutex& () const noexcept { return *lock; }

private:
    std::shared_ptr<std::mutex> lock;
    void* streambuf;
    streambuf_locks* locks;
};
class streambuf_locks {
private:
    streambuf_locks() = default;

public:
    static streambuf_locks& init() {
        static streambuf_locks locks;
        return locks;
    }
    streambuf_lock_proxy get_lock(void* streambuf) {
        if (streambuf == nullptr) return {};
        guard lk{mx};
        std::shared_ptr<std::mutex> result{};
        auto& mxptr = thelocks[streambuf];
        if (mxptr.expired()) {
            mxptr = result = std::make_shared<std::mutex>();
        } else {
            result = mxptr.lock();
        }
        return {std::move(result), streambuf, this};
    }
    static streambuf_lock_proxy get(void* streambuf) { return init().get_lock(streambuf); }
    void release_lock(std::shared_ptr<std::mutex>& sp, void* streambuf) {
        if (sp == nullptr && streambuf == nullptr) return;
        guard lx{mx};
        auto iter = thelocks.find(streambuf);
        sp.reset();
        if (iter != thelocks.end() && iter->second.expired()) {
            thelocks.erase(iter);
        }
    }
    auto size() const {
        guard lx{mx};
        return thelocks.size();
    }

private:
    mutable std::mutex mx{};
    std::unordered_map<void*, std::weak_ptr<std::mutex>> thelocks{};
    using guard = std::lock_guard<std::mutex>;
};
inline streambuf_lock_proxy::~streambuf_lock_proxy() noexcept {
    if (locks) locks->release_lock(this->lock, this->streambuf);
}

}  // namespace inferior::detail
#endif  // INFERIOR_STREAMBUF_LOCKS_HPP_
