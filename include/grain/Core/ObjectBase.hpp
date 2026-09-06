#pragma once

#include <cstdint>

namespace Grain {

class ObjectBase {
public:
    using Id = std::uint64_t;

    ObjectBase();

    virtual ~ObjectBase() = default;

    ObjectBase(const ObjectBase&) = delete;

    ObjectBase& operator=(const ObjectBase&) = delete;

    ObjectBase(ObjectBase&&) = delete;

    ObjectBase& operator=(ObjectBase&&) = delete;

    [[nodiscard]]
    Id id() const noexcept {
        return id_;
    }

private:
    Id id_;
};

} // namespace Grain