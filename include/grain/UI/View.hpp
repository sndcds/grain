#pragma once

#include <grain/UI/UIElement.hpp>
#include <grain/UI/Component.hpp>
#include <grain/Graphics/GraphicContext.hpp>
#include <grain/Input/Event.hpp>

#include <memory>
#include <utility>
#include <vector>
#include <concepts>


namespace Grain {

class Window;
class Component;

namespace Platform {
class View;
}


class View : public UIElement {
public:
    View();
    View(const View&) = delete;

    virtual ~View();

    auto operator = (const View&) -> View& = delete;
    View(View&&) = delete;
    auto operator = (View&&) -> View& = delete;

    [[nodiscard]]
    auto type() const -> UIElementType override {
        return UIElementType::View;
    }

    [[nodiscard]] auto bounds() const -> const Rectd& { return bounds_; }
    void setBounds(const Rectd& bounds) { bounds_ = bounds; }

	template <typename T, typename... Args>
	requires std::derived_from<T, UIElement>
	auto addChild(Args&&... args) -> T& {
	    static_assert(std::is_base_of_v<UIElement, T>);

	    auto child = std::make_unique<T>(
    	    std::forward<Args>(args)...
    	);

	    T& result = *child;
    	result.parent_ = this;

	    children_.push_back(std::move(child));

    	requestRedraw();

	    return result;
	}


    auto elementCount_() -> size_t { return children_.size(); }

    void drawComponents(GraphicContext& gc, const Rectd& dirty_rect);
    virtual void drawContent(GraphicContext& gc, const Rectd& dirty_rect);

	void draw(
		GraphicContext& gc,
		const Rectd& dirty_rect
	) override;

    virtual void requestRedraw();
    void requestRedraw(const Rectd& dirty_rect);

	void doSizing() override;

    virtual void handleMouseEvent(const Event& event);
    virtual void handleKeyEvent(const Event& event);
    virtual void handleEvent(const Event& event);

    [[nodiscard]] auto mousePos() -> Vec2d { return queryMousePos(); }

	template <typename Visitor>
	auto traverse(Visitor&& visitor) -> bool {
    	return traverseImpl(*this, visitor, false);
    }

	template <typename Visitor>
	auto traverseReverse(Visitor&& visitor) -> bool {
    	return traverseImpl(*this, visitor, true);
    }

private:
    class Impl;
    std::unique_ptr<Impl> impl_;


	template <typename Visitor>
	static auto traverseImpl(
		UIElement& element,
		Visitor& visitor,
		bool reverse
	) -> bool {
		// Visit current element.
		// Returning false stops the entire traversal.
		if (!std::invoke(visitor, element)) {
			return false;
		}

		// Components are leaves; only Views have children.
		if (element.type() != UIElementType::View) {
			return true;
		}

		auto& view = static_cast<View&>(element);

		if (reverse) {
			for (auto it = view.children_.rbegin();
				 it != view.children_.rend();
				 ++it) {
				if (!traverseImpl(**it, visitor, reverse)) {
					return false;
				}
				 }
		} else {
			for (auto& child : view.children_) {
				if (!traverseImpl(*child, visitor, reverse)) {
					return false;
				}
			}
		}

		return true;
	}

    void attach();

    [[nodiscard]] auto platformView() -> Platform::View*;

    void updateHoveredComponent(Component* component);
    void updateFocusedComponent(Component* component);
    auto hitTest(const Vec2d& pos) -> Component*;
    [[nodiscard]] auto queryMousePos() const -> Vec2d;

    Rectd bounds_;
    Vec2d mouse_pos_;

	std::vector<std::unique_ptr<UIElement>> children_;

    // Interaction states
    Component* hovered_component_ = nullptr;
    Component* focused_component_ = nullptr;
    Component* mouse_capture_component_ = nullptr;

    friend class Window;
};

} // namespace Grain