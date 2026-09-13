#pragma once

#include "Gtk4App.hpp"

namespace Grain::Platform {

class Gtk4Window final : public Window {
public:
    Gtk4Window(WindowId id, GtkApplication* application, std::string_view title,
               int width, int height, std::shared_ptr<Gtk4Callbacks> callbacks);
    ~Gtk4Window() override;

    void show() override;
    void setRootView(Grain::View* view) override;

    [[nodiscard]] WindowId id() const noexcept { return windowId_; }
    [[nodiscard]] GtkWindow* nativeWindow() const noexcept { return window_; }

private:
    void emit(Event event) noexcept;
    static void draw(GtkWidget*, GtkSnapshot* snapshot, gpointer data) noexcept;
    static void resize(int width, int height, gpointer data) noexcept;
    static gboolean closeRequest(GtkWindow*, gpointer data) noexcept;
    static void focusChanged(GObject*, GParamSpec*, gpointer data) noexcept;

    WindowId windowId_;
    GtkWindow* window_ = nullptr;
    GtkWidget* canvas_ = nullptr;
    Grain::View* view_ = nullptr;
    std::shared_ptr<Gtk4Callbacks> callbacks_;
    bool closed_ = false;
};

} // namespace Grain::Platform
