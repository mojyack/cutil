#pragma once
#include <poll.h>
#include <sys/eventfd.h>

#include "fd.hpp"

#ifdef CUTIL_NS
namespace CUTIL_NS {
#endif

namespace {
class EventFileDescriptor {
  private:
    const FileDescriptor fd;

  public:
    auto notify() const -> void {
        fd.write(uint64_t(1));
    }

    auto consume() const -> uint64_t {
        const auto v = fd.read<uint64_t>();
        return v.has_value() ? *v : 0;
    }

    auto wait() const -> void {
        auto pfd = pollfd{fd.as_handle(), POLLIN, 0};
        while(true) {
            poll(&pfd, 1, -1);
            if(pfd.revents & POLLIN) {
                consume();
                return;
            }
        }
        return;
    }

    operator int() const {
        return fd.as_handle();
    }

    EventFileDescriptor() : fd(eventfd(0, 0)) {}
};
} // namespace

#ifdef CUTIL_NS
}
#endif
