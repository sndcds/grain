#include "Gtk4App.hpp"
#include "Gtk4Window.hpp"

#include <stdexcept>
#include <string>
#include <utility>

namespace Grain::Platform {

Gtk4App::Gtk4App() {
    if (!gtk_init_check()) {
        throw std::runtime_error("Grain: unable to initialize GTK4 display");
    }

    // Grain creates its windows before run(), so register before creating them.
    application_ = gtk_application_new(nullptr, G_APPLICATION_NON_UNIQUE);
    g_signal_connect(application_, "activate", G_CALLBACK(+[](GtkApplication*, gpointer) {}), nullptr);
    GError* error = nullptr;
    if (!g_application_register(G_APPLICATION(application_), nullptr, &error)) {
        const std::string message = error->message;
        g_error_free(error);
        g_object_unref(application_);
        throw std::runtime_error("Grain: unable to register GTK4 application: " + message);
    }
    callbacks_->application = application_;
    callbacks_->eventHandler = [this](const Event& event) { emitEvent(event); };
}

Gtk4App::~Gtk4App() {
    callbacks_->eventHandler = {};
    callbacks_->application = nullptr;
    g_object_unref(application_);
}

std::unique_ptr<Window> Gtk4App::createWindow(
    WindowId id, std::string_view title, int width, int height) {
    return std::make_unique<Gtk4Window>(id, application_, title, width, height, callbacks_);
}

void Gtk4App::run() {
    if (!callbacks_->error) {
        g_application_run(G_APPLICATION(application_), 0, nullptr);
    }
    if (callbacks_->error) {
        std::rethrow_exception(std::exchange(callbacks_->error, {}));
    }
}

void Gtk4App::quit() {
    g_application_quit(G_APPLICATION(application_));
}

std::unique_ptr<App> createApp() {
    return std::make_unique<Gtk4App>();
}

} // namespace Grain::Platform
