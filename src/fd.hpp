#pragma once
#include <cstdint>
#include <optional>
#include <utility>
#include <vector>

#include <unistd.h>

#ifdef CUTIL_NS
namespace CUTIL_NS {
#endif

namespace {
class FileDescriptor {
  private:
    int fd = -1;

  public:
    auto read(void* const data, const size_t size) const -> bool {
        auto len = size_t(0);
        while(len < size) {
            const auto n = ::read(fd, (uint8_t*)data + len, size - len);
            if(n == -1) {
                return false;
            }
            len += n;
        }
        return true;
    }

    auto read(const size_t size) const -> std::optional<std::vector<std::byte>> {
        auto r = std::vector<std::byte>(size);
        return read(r.data(), size) ? std::make_optional(r) : std::nullopt;
    }

    template <class T>
    auto read() const -> std::optional<T> {
        auto r = T();
        return read(&r, sizeof(T)) ? std::make_optional(r) : std::nullopt;
    }

    auto read_sized() const -> std::optional<std::vector<std::byte>> {
        const auto size = read<size_t>();
        if(!size.has_value()) {
            return std::nullopt;
        }
        auto r = read(*size);
        if(!r.has_value()) {
            return std::nullopt;
        }
        return r;
    }

    template <class T>
    auto write(const T data) const -> bool {
        const auto size = sizeof(T);
        return write(&data, size);
    }

    auto write(const void* const data, const size_t size) const -> bool {
        auto wrote = size_t(0);
        while(wrote != size) {
            const auto r = ::write(fd, data, size);
            if(r == -1) {
                return false;
            }
            wrote += r;
        }
        return true;
    }

    auto close() -> void {
        if(fd != -1) {
            ::close(fd);
        }
    }

    auto release() -> int {
        return std::exchange(fd, -1);
    }

    auto clone() -> FileDescriptor {
        return dup(fd);
    }

    auto as_handle() const -> int {
        return fd;
    }

    auto operator=(const int new_fd) -> FileDescriptor& {
        close();
        fd = new_fd;
        return *this;
    }

    auto operator=(FileDescriptor&& o) -> FileDescriptor& {
        *this = o.release();
        return *this;
    }

    FileDescriptor() : fd(-1) {}

    FileDescriptor(const int fd) : fd(fd) {}

    FileDescriptor(FileDescriptor&& o) {
        *this = std::move(o);
    }

    ~FileDescriptor() {
        close();
    }
};
} // namespace

#ifdef CUTIL_NS
}
#endif
