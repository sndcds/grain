#pragma once

#include <cassert>
#include <cstddef>
#include <memory>
#include <utility>

namespace Grain {

/**
 * @brief A dynamically sized contiguous container.
 *
 * List owns its elements and stores them in contiguous memory.
 *
 * T does not need to be default constructible. Elements are constructed
 * directly into uninitialized storage and destroyed explicitly.
 */
template <typename T>
class List {
public:
    using ValueType = T;
    using SizeType = std::size_t;
    using Iterator = T*;
    using ConstIterator = const T*;

    List() = default;

    explicit List(SizeType capacity) {
        reserve(capacity);
    }

    ~List() {
        destroy_elements();
        deallocate();
    }


    //--------------------------------------------------------------------------
    //  Copy.
    //--------------------------------------------------------------------------

    List(const List& other)
        requires std::copy_constructible<T> {
        if (other.size_ == 0) {
            return;
        }

        reserve(other.size_);

        SizeType constructed = 0;

        try {
            for (; constructed < other.size_; ++constructed) {
                std::construct_at(
                    data_ + constructed,
                    other.data_[constructed]
                    );
            }
        } catch (...) {
            std::destroy_n(data_, constructed);
            deallocate();
            throw;
        }

        size_ = other.size_;
    }

    List(const List&)
        requires (!std::copy_constructible<T>) = delete;

    auto operator = (const List& other) -> List&
        requires std::copy_constructible<T> {
        if (this == &other) {
            return *this;
        }

        List temporary(other);
        swap(temporary);

        return *this;
    }

    auto operator = (const List&) -> List&
        requires (!std::copy_constructible<T>) = delete;


    //--------------------------------------------------------------------------
    //  Move.
    //--------------------------------------------------------------------------

    List(List&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    auto operator = (List&& other) noexcept -> List& {
        if (this == &other) {
            return *this;
        }

        destroy_elements();
        deallocate();

        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;

        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;

        return *this;
    }


    //--------------------------------------------------------------------------
    //  Capacity.
    //--------------------------------------------------------------------------

    [[nodiscard]] auto size() const -> SizeType { return size_; }
    [[nodiscard]] auto capacity() const -> SizeType { return capacity_; }
    [[nodiscard]] auto empty() const -> bool { return size_ == 0; }

    void reserve(SizeType new_capacity) {
        if (new_capacity <= capacity_) {
            return;
        }

        T* new_data = allocator_.allocate(new_capacity);

        SizeType constructed = 0;

        try {
            for (; constructed < size_; ++constructed) {
                std::construct_at(
                    new_data + constructed,
                    std::move_if_noexcept(data_[constructed])
                    );
            }
        } catch (...) {
            std::destroy_n(new_data, constructed);
            allocator_.deallocate(new_data, new_capacity);
            throw;
        }

        destroy_elements();
        deallocate();

        data_ = new_data;
        capacity_ = new_capacity;
    }


    //--------------------------------------------------------------------------
    //  Modifiers.
    //--------------------------------------------------------------------------

    void clear() {
        destroy_elements();
        size_ = 0;
    }

    void pop_back() {
        assert(size_ > 0);

        --size_;
        std::destroy_at(data_ + size_);
    }

    void removeAtIndex(SizeType index) {
        assert(index < size_);

        for (SizeType i = index; i + 1 < size_; ++i) {
            data_[i] = std::move(data_[i + 1]);
        }

        pop_back();
    }

    void push_back(const T& value)
        requires std::copy_constructible<T> {
        emplace_back(value);
    }

    void push_back(const T&)
        requires (!std::copy_constructible<T>) = delete;

    void push_back(T&& value) {
        emplace_back(std::move(value));
    }

    template <typename... Args>
        requires std::constructible_from<T, Args...>
    auto emplace_back(Args&&... args) -> T& {
        if (size_ == capacity_) {
            reserve(growth_capacity());
        }

        T* element = data_ + size_;

        std::construct_at(
            element,
            std::forward<Args>(args)...
            );

        ++size_;

        return *element;
    }


    //--------------------------------------------------------------------------
    //  Element access.
    //--------------------------------------------------------------------------

    auto operator [] (SizeType index) -> T& {
        assert(index < size_);
        return data_[index];
    }

    auto operator [] (SizeType index) const -> const T& {
        assert(index < size_);
        return data_[index];
    }

    auto front() -> T& {
        assert(size_ > 0);
        return data_[0];
    }

    auto front() const -> const T& {
        assert(size_ > 0);
        return data_[0];
    }

    auto back() -> T& {
        assert(size_ > 0);
        return data_[size_ - 1];
    }

    auto back() const -> const T& {
        assert(size_ > 0);
        return data_[size_ - 1];
    }


    //--------------------------------------------------------------------------
    //  Raw storage.
    //--------------------------------------------------------------------------

    auto data() -> T* { return data_; }
    auto data() const -> const T* { return data_; }


    //--------------------------------------------------------------------------
    //  Iteration.
    //--------------------------------------------------------------------------

    auto begin() -> Iterator { return data_; }
    auto begin() const -> ConstIterator { return data_; }
    auto cbegin() const -> ConstIterator { return data_; }
    auto end() -> Iterator { return data_ + size_; }
    auto end() const -> ConstIterator { return data_ + size_; }
    auto cend() const -> ConstIterator { return data_ + size_; }


    //--------------------------------------------------------------------------
    //  Miscellaneous.
    //--------------------------------------------------------------------------

    void swap(List& other) noexcept {
        using std::swap;

        swap(data_, other.data_);
        swap(size_, other.size_);
        swap(capacity_, other.capacity_);
    }

private:
    [[nodiscard]]
    auto growth_capacity() const -> SizeType {
        if (capacity_ == 0) {
            return 8;
        }

        return capacity_ * 2;
    }

    void destroy_elements() {
        std::destroy_n(data_, size_);
    }

    void deallocate() {
        if (data_ != nullptr) {
            allocator_.deallocate(data_, capacity_);
            data_ = nullptr;
            capacity_ = 0;
        }
    }

    T* data_ = nullptr;
    SizeType size_ = 0;
    SizeType capacity_ = 0;

    [[no_unique_address]]
    std::allocator<T> allocator_;
};

} // namespace Grain
