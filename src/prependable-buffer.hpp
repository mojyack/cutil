#pragma once
#include <span>
#include <vector>

// PrependableBuffer: bytes array, data can be prepended without reallocation

struct PrependableBuffer {
    constexpr static auto preallocation_size = 32;

    std::vector<std::byte> storage;
    size_t                 body_head = 0;

    auto size() const -> size_t;
    auto resize(size_t size) -> void;
    auto body() -> std::span<std::byte>;
    auto body() const -> std::span<const std::byte>;
    auto enlarge(size_t size) -> std::span<std::byte>;
    auto enlarge_forward(size_t size) -> std::span<std::byte>;
    template <class T>
    auto append_object(const T& obj) -> void;
    template <class T>
    auto append_array(const T& array) -> void;
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

template <class T>
auto PrependableBuffer::append_object(const T& obj) -> void {
    auto span = enlarge(sizeof(T));
    std::memcpy(span.data(), &obj, span.size());
}

template <class T>
auto PrependableBuffer::append_array(const T& array) -> void {
    static_assert(sizeof(array[0]) == 1);
    auto span = enlarge(array.size());
    std::memcpy(span.data(), array.data(), span.size());
}
