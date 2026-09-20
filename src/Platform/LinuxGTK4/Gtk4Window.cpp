#include "Gtk4Window.hpp"
#include "Gtk4GraphicContext.hpp"

#include <../../../include/grain/Platform/View.hpp>

#include <algorithm>
#include <string>
#include <utility>

namespace {

struct GrainGtk4Canvas {
    GtkWidget parent;
    void (*draw)(GtkWidget*, GtkSnapshot*, gpointer) noexcept;
    void (*resize)(int, int, gpointer) noexcept;
    gpointer data;
};

struct GrainGtk4CanvasClass {
    GtkWidgetClass parent;
};

G_DEFINE_TYPE(GrainGtk4Canvas, grain_gtk4_canvas, GTK_TYPE_WIDGET)

void grain_gtk4_canvas_class_init(GrainGtk4CanvasClass* klass) {
    auto* widgetClass = GTK_WIDGET_CLASS(klass);
    widgetClass->snapshot = +[](GtkWidget* widget, GtkSnapshot* snapshot) {
        auto* canvas = reinterpret_cast<GrainGtk4Canvas*>(widget);
        if (canvas->draw != nullptr) {
            canvas->draw(widget, snapshot, canvas->data);
        }
    };
    widgetClass->size_allocate = +[](GtkWidget* widget, int width, int height, int) {
        auto* canvas = reinterpret_cast<GrainGtk4Canvas*>(widget);
        if (canvas->resize != nullptr) {
            canvas->resize(width, height, canvas->data);
        }
    };
}

void grain_gtk4_canvas_init(GrainGtk4Canvas* canvas) {
    gtk_widget_set_overflow(GTK_WIDGET(canvas), GTK_OVERFLOW_HIDDEN);
}

} // namespace

namespace Grain::Platform {

Gtk4Window::Gtk4Window(WindowId id, GtkApplication* application, std::string_view title,
                       int width, int height, std::shared_ptr<Gtk4Callbacks> callbacks)
    : windowId_(id), callbacks_(std::move(callbacks)) {
    window_ = GTK_WINDOW(gtk_application_window_new(application));
    // Keep handles valid after GTK destroys a window in response to a close request.
    g_object_ref(window_);
    gtk_window_set_title(window_, std::string(title).c_str());
    gtk_window_set_default_size(window_, std::max(width, 1), std::max(height, 1));

    auto* canvas = static_cast<GrainGtk4Canvas*>(g_object_new(grain_gtk4_canvas_get_type(), nullptr));
    canvas_ = GTK_WIDGET(canvas);
    canvas->draw = draw;
    canvas->resize = resize;
    canvas->data = this;
    g_object_ref_sink(canvas_);
    gtk_window_set_child(window_, canvas_);
    g_signal_connect(window_, "close-request", G_CALLBACK(closeRequest), this);
    g_signal_connect(window_, "notify::is-active", G_CALLBACK(focusChanged), this);
}

Gtk4Window::~Gtk4Window() {
    view_ = nullptr;
    auto* canvas = reinterpret_cast<GrainGtk4Canvas*>(canvas_);
    canvas->draw = nullptr;
    canvas->resize = nullptr;
    canvas->data = nullptr;
    g_signal_handlers_disconnect_by_data(window_, this);
    gtk_window_destroy(window_);
    g_object_unref(canvas_);
    g_object_unref(window_);
}

void Gtk4Window::show() {
    if (!closed_) {
        gtk_window_present(window_);
    }
}

void Gtk4Window::setRootView(Grain::View* view) {
    view_ = view;
    if (!closed_) {
        gtk_widget_queue_draw(canvas_);
    }
}

void Gtk4Window::emit(Event event) noexcept {
    event.windowId = windowId_;
    callbacks_->invoke([&] {
        if (callbacks_->eventHandler) {
            callbacks_->eventHandler(event);
        }
    });
}

void Gtk4Window::draw(GtkWidget* widget, GtkSnapshot* snapshot, gpointer data) noexcept {
    auto& self = *static_cast<Gtk4Window*>(data);
    self.callbacks_->invoke([&] {
        if (self.view_ != nullptr && !self.closed_) {
            self.view_->setBounds({0, 0, static_cast<double>(gtk_widget_get_width(widget)),
                                  static_cast<double>(gtk_widget_get_height(widget))});
            Gtk4GraphicContext graphics(snapshot);
            self.view_->draw(graphics);
        }
    });
}

void Gtk4Window::resize(int width, int height, gpointer data) noexcept {
    auto& self = *static_cast<Gtk4Window*>(data);
    Event event;
    event.type = EventType::WindowResize;
    event.width = width;
    event.height = height;
    self.emit(event);
}

gboolean Gtk4Window::closeRequest(GtkWindow*, gpointer data) noexcept {
    auto& self = *static_cast<Gtk4Window*>(data);
    if (!self.closed_) {
        self.closed_ = true;
        Event event;
        event.type = EventType::WindowClose;
        self.emit(event);
    }
    // GTK destroys the native window and quits after the last application window closes.
    return FALSE;
}

void Gtk4Window::focusChanged(GObject*, GParamSpec*, gpointer data) noexcept {
    auto& self = *static_cast<Gtk4Window*>(data);
    Event event;
    event.type = gtk_window_is_active(self.window_) ? EventType::WindowFocusGained : EventType::WindowFocusLost;
    self.emit(event);
}

} // namespace Grain::Platform
