#pragma once

#include <grain/Graphics/GraphicContext.hpp>
#include <grain/Input/Event.hpp>
#include <grain/UI/Component.hpp>

#include <memory>

namespace Grain {

class Window;

namespace Platform {
class View;
}

class View {
public:
    View();

    View(const View&) = delete;
    View& operator=(const View&) = delete;

    View(View&&) = delete;
    View& operator=(View&&) = delete;

    virtual ~View();

    [[nodiscard]]
    const Rectd& bounds() const noexcept {
        return bounds_;
    }

    void setBounds(const Rectd& bounds) noexcept {
        bounds_ = bounds;
    }

    [[nodiscard]]
    View* parent() const noexcept {
        return parent_;
    }

    template<typename T, typename... Args>
    T& addComponent(Args&&... args) {
        auto component = std::make_unique<T>(
            this,
            std::forward<Args>(args)...
        );

        T& result = *component;
        components_.push_back(std::move(component));

        requestRedraw();

        return result;
    }

    void drawComponents(GraphicContext& gc) {
        for (auto& component : components_) {
            if (component->visible()) {
                component->draw(gc);
            }
        }
    }

    virtual void draw(GraphicContext& context);
    virtual void drawContent(GraphicContext& gc);
    virtual void handleEvent(const Event& event);
    virtual void requestRedraw();

private:
    class Impl;

    std::unique_ptr<Impl> impl_;

    void attach();

    [[nodiscard]]
    Platform::View* platformView() noexcept;

    Rectd bounds_{};
    View* parent_ = nullptr;

    std::vector<std::unique_ptr<Component>> components_;

    friend class Window;
};

} // namespace Grain