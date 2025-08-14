#pragma once
#include <span>
#include <vector>

// PrependableBuffer: bytes array, data can be prepended without reallocation

struct PrependableBuffer {
    constexpr static auto preallocation_size = 32;

    std::vector<std::byte> storage;
    size_t                 body_head = 0;

    // <- forward     backward ->
    // 0 ..    .. head ..    .. N
    // | prealloc |     body    |

    // get body size (i.e. body().size())
    auto size() const -> size_t;
    // resize body
    auto resize(size_t size) -> void;
    // get span to body
    auto body() -> std::span<std::byte>;
    auto body() const -> std::span<const std::byte>;
    // move body end backward
    auto enlarge(size_t size) -> std::span<std::byte>;
    // move body start forward
    auto enlarge_forward(size_t size) -> std::span<std::byte>;
    // move body end forward
    auto shrink(size_t size) -> void;
    // move body start backward
    auto shrink_backward(size_t size) -> void;
    // enlarge then memcpy
    template <class T>
    auto append_object(const T& obj) -> PrependableBuffer&&;
    template <class T>
    auto append_array(const T& array) -> PrependableBuffer&&;
    // enlarge_forward then memcpy
    template <class T>
    auto prepend_object(const T& obj) -> PrependableBuffer&&;
};

inline auto PrependableBuffer::size() const -> size_t {
    return storage.size() - body_head;
}

inline auto PrependableBuffer::resize(const size_t size) -> void {
    storage.resize(body_head + size);
}

inline auto PrependableBuffer::body() -> std::span<std::byte> {
    return std::span(storage).subspan(body_head);
}

inline auto PrependableBuffer::body() const -> std::span<const std::byte> {
    return ((PrependableBuffer*)this)->body();
}

inline auto PrependableBuffer::enlarge(const size_t size) -> std::span<std::byte> {
    const auto prev_size = this->size();
    if(body_head == 0) {
        body_head = preallocation_size;
        storage.resize(body_head + size);
    } else {
        storage.resize(storage.size() + size);
    }
    return std::span(storage).subspan(body_head + prev_size);
}

inline auto PrependableBuffer::enlarge_forward(const size_t size) -> std::span<std::byte> {
    if(size > body_head) {
        // run out of preallocated buffer, need resize
        const auto curr_size    = this->size();
        const auto new_head     = preallocation_size + size;
        const auto head_diff    = new_head - body_head;
        const auto new_capacity = storage.size() + head_diff;
        storage.resize(new_capacity);
        std::memcpy(storage.data() + new_head, storage.data() + body_head, curr_size);
        body_head = new_head;
    }
    body_head -= size;
    return std::span(storage).subspan(body_head, size);
}

inline auto PrependableBuffer::shrink(const size_t size) -> void {
    storage.resize(storage.size() - std::min(size, this->size()));
}

inline auto PrependableBuffer::shrink_backward(const size_t size) -> void {
    body_head = std::min(body_head + size, storage.size());
}

template <class T>
auto PrependableBuffer::append_object(const T& obj) -> PrependableBuffer&& {
    auto span = enlarge(sizeof(T));
    std::memcpy(span.data(), &obj, span.size());
    return std::move(*this);
}

template <class T>
auto PrependableBuffer::append_array(const T& array) -> PrependableBuffer&& {
    static_assert(sizeof(array[0]) == 1);
    auto span = enlarge(array.size());
    std::memcpy(span.data(), array.data(), span.size());
    return std::move(*this);
}

template <class T>
auto PrependableBuffer::prepend_object(const T& obj) -> PrependableBuffer&& {
    auto span = enlarge_forward(sizeof(T));
    std::memcpy(span.data(), &obj, span.size());
    return std::move(*this);
}
