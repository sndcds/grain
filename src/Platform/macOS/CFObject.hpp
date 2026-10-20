#pragma once

#include <CoreFoundation/CoreFoundation.h>

namespace Grain {

template <typename T>
class CFObject {
public:
    constexpr CFObject() = default;

    explicit CFObject(T object)
        : object_(object) {
    }

    ~CFObject() {
        reset();
    }

    CFObject(const CFObject&) = delete;
    CFObject& operator=(const CFObject&) = delete;

    CFObject(CFObject&& other)
        : object_(other.object_) {
        other.object_ = nullptr;
    }

    CFObject& operator=(CFObject&& other) {
        if (this != &other) {
            reset();

            object_ = other.object_;
            other.object_ = nullptr;
        }

        return *this;
    }

    [[nodiscard]] T get() const { return object_; }

    [[nodiscard]] explicit operator bool() const {
        return object_ != nullptr;
    }

    T release() {
        T object = object_;
        object_ = nullptr;
        return object;
    }

    void reset(T object = nullptr) {
        if (object_ != nullptr) {
            CFRelease(object_);
        }

        object_ = object;
    }

private:
    T object_ = nullptr;
};

} // namespace Grain