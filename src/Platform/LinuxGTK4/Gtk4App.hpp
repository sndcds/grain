#pragma once

#include "../Platform.hpp"

#include <gtk/gtk.h>

#include <exception>

namespace Grain::Platform {

// Windows may outlive the platform app. Disconnect forwarding when it goes away.
struct Gtk4Callbacks {
    GtkApplication* application = nullptr;
    EventHandler eventHandler;
    std::exception_ptr error;

    template<typename Callback>
    void invoke(Callback&& callback) noexcept {
        if (error) {
            return;
        }
        try {
            callback();
        } catch (...) {
            // Never unwind a C++ exception through GTK's C callbacks.
            error = std::current_exception();
            if (application != nullptr) {
                g_application_quit(G_APPLICATION(application));
            }
        }
    }
};

class Gtk4App final : public App {
public:
    Gtk4App();
    ~Gtk4App() override;

    std::unique_ptr<Window> createWindow(
        WindowId id, std::string_view title, int width, int height) override;

    void run() override;
    void quit() override;

private:
    GtkApplication* application_ = nullptr;
    std::shared_ptr<Gtk4Callbacks> callbacks_ = std::make_shared<Gtk4Callbacks>();
};

} // namespace Grain::Platform
