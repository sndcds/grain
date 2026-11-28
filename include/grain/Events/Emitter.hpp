#pragma once

#include <sigslot/signal.hpp>

#include <utility>

namespace Grain::Events {

template<typename... Args>
class Emitter {
public:
    using Connection = sigslot::connection;
    using ScopedConnection = sigslot::scoped_connection;

    Emitter() = default;

    Emitter(const Emitter&) = delete;
    Emitter& operator = (const Emitter&) = delete;

    Emitter(Emitter&&) = delete;
    Emitter& operator = (Emitter&&) = delete;

    template<typename... ConnectArgs>
    [[nodiscard]]
    auto connect(ConnectArgs&&... args) {
        return signal_.connect(
            std::forward<ConnectArgs>(args)...
        );
    }

    void emit(Args... args) {
        signal_(args...);
    }

    void disconnectAll() {
        signal_.disconnect_all();
    }

    [[nodiscard]]
    auto slotCount() {
        return signal_.slot_count();
    }

private:
    sigslot::signal_st<Args...> signal_;
};

} // namespace Grain::Events