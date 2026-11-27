
#include <grain/UI/Popup.hpp>

#include <grain/UI/View.hpp>
#include <grain/Platform/Window.hpp>
#include <grain/UI/LayoutEngine.hpp>

#include <grain/Platform/Platform.hpp>

#include <utility>

namespace Grain {

class Popup::Impl {
public:
    Window* parent_ = nullptr;

    std::unique_ptr<View> content_;
    std::unique_ptr<Platform::Popup> platform_popup_;

    LayoutEngine layout_engine_;

    Rectd anchor_;
    Sized size_;
    PopupOptions options_;

    bool initialized_ = false;

    Impl(
        Window& parent,
        std::unique_ptr<View> content,
        const Rectd& anchor,
        const Sized& size,
        const PopupOptions& options
    )
        : parent_(&parent),
          content_(std::move(content)),
          platform_popup_(Platform::createPopup()),
          anchor_(anchor),
          size_(size),
          options_(options)
    {
    }

    ~Impl() {
        if (content_ && content_->platformView()) {
            content_->platformView()->setEventHandler({});
        }

        // Destroy the native popup before its content view.
        platform_popup_.reset();
    }

    void initialize() {
        if (initialized_ ||
            parent_ == nullptr ||
            content_ == nullptr ||
            platform_popup_ == nullptr) {
            return;
        }

        content_->setLayoutBounds({
            0,
            0,
            size_.width,
            size_.height
        });

        content_->attach();

        auto* platform_view = content_->platformView();

        if (platform_view == nullptr) {
            return;
        }

        platform_view->setEventHandler(
            [this](const Event& event) {
                dispatchEvent(event);
            }
        );

        initialized_ = true;
    }

    void dispatchEvent(const Event& event) {
        if (content_ == nullptr) {
            return;
        }

        switch (event.type_) {
            case EventType::MouseMove:
            case EventType::MouseButtonDown:
            case EventType::MouseButtonUp:
            case EventType::MouseWheel:
                content_->dispatchMouseEvent(event);
                break;

            case EventType::KeyDown:
            case EventType::KeyUp:
            case EventType::ModifiersChanged:
                content_->handleKeyEvent(event);
                break;

            default:
                content_->handleEvent(event);
                break;
        }
    }

    void updateContentBounds() {
        if (content_ == nullptr) {
            return;
        }

        content_->setLayoutBounds({
            0,
            0,
            size_.width,
            size_.height
        });
    }

    void show() {
        initialize();

        if (!initialized_) {
            return;
        }

        auto* parent_platform = parent_->platformWindow();
        auto* content_platform = content_->platformView();

        if (parent_platform == nullptr ||
            content_platform == nullptr) {
            return;
        }

        platform_popup_->show(
            *parent_platform,
            content_platform,
            anchor_,
            size_,
            options_
        );
    }
};


Popup::Popup(
    Window& parent,
    std::unique_ptr<View> content,
    const Rectd& anchor,
    const Sized& size,
    const PopupOptions& options
)
    : impl_(std::make_unique<Impl>(
        parent,
        std::move(content),
        anchor,
        size,
        options
    ))
{
}


Popup::~Popup() = default;


void Popup::show() {
    if (impl_) {
        impl_->show();
    }
}


void Popup::hide() {
    if (impl_ && impl_->platform_popup_) {
        impl_->platform_popup_->hide();
    }
}


void Popup::reposition(const Rectd& anchor) {
    if (!impl_) {
        return;
    }

    impl_->anchor_ = anchor;

    if (impl_->platform_popup_) {
        impl_->platform_popup_->reposition(anchor);
    }
}


void Popup::resize(const Sized& size) {
    if (!impl_) {
        return;
    }

    impl_->size_ = size;
    impl_->updateContentBounds();

    if (impl_->platform_popup_) {
        impl_->platform_popup_->resize(size);
    }
}


auto Popup::isVisible() const -> bool {
    return impl_ &&
           impl_->platform_popup_ &&
           impl_->platform_popup_->isVisible();
}


auto Popup::contentView() -> View& {
    return *impl_->content_;
}

} // namespace Grain
