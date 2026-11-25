
#include <Grain.hpp>

#include <algorithm>
#include <cstdint>
#include <string>

using namespace Grain;


namespace Grain::Styles {

inline auto create(UIStyles& styles) -> void {

    //--------------------------------------------------------------------------
    //  Root: vertical layout (content + footer)
    //--------------------------------------------------------------------------

    LayoutViewStyle root;
    root.direction = LayoutDirection::TopToBottom;
    root.element.padding = {0, 0, 0, 0};
    root.element.child_gap = 0;

    styles.defineViewStyle("root", root);


    //--------------------------------------------------------------------------
    //  Content: horizontal layout (left + center + right)
    //--------------------------------------------------------------------------

    LayoutViewStyle content;
    content.direction = LayoutDirection::LeftToRight;
    content.element.padding = {0, 0, 0, 0};
    content.element.child_gap = 0;

    content.element.sizing.width.type = LayoutSizingType::Grow;
    content.element.sizing.height.type = LayoutSizingType::Grow;

    styles.defineViewStyle("content", content);


    //--------------------------------------------------------------------------
    //  Center view: fills available space
    //--------------------------------------------------------------------------

    LayoutViewStyle center_view;
    center_view.direction = LayoutDirection::TopToBottom;
    center_view.element.padding = {8, 8, 8, 8};

    center_view.element.sizing.width.type = LayoutSizingType::Grow;
    center_view.element.sizing.height.type = LayoutSizingType::Grow;
    center_view.element.sizing.width.min_value = 200;

    styles.defineViewStyle("center-view", center_view);


    //--------------------------------------------------------------------------
    //  Left panel: 200–300 px wide
    //--------------------------------------------------------------------------

    LayoutViewStyle left_panel;
    left_panel.direction = LayoutDirection::TopToBottom;
    left_panel.element.padding = {8, 8, 8, 8};
    left_panel.element.child_gap = 2;

    left_panel.element.sizing.width.type = LayoutSizingType::Fixed;
    left_panel.element.sizing.width.min_value = 200;
    left_panel.element.sizing.width.max_value = 300;

    left_panel.element.sizing.height.type = LayoutSizingType::Grow;

    styles.defineViewStyle("left-panel", left_panel);


    //--------------------------------------------------------------------------
    //  Right panel: 200 px wide
    //--------------------------------------------------------------------------

    LayoutViewStyle right_panel;
    right_panel.direction = LayoutDirection::TopToBottom;
    right_panel.element.padding = {8, 8, 8, 8};

    right_panel.element.sizing.width.type = LayoutSizingType::Fixed;
    right_panel.element.sizing.width.min_value = 200;
    right_panel.element.sizing.width.max_value = 200;

    right_panel.element.sizing.height.type = LayoutSizingType::Grow;

    styles.defineViewStyle("right-panel", right_panel);


    //--------------------------------------------------------------------------
    //  Footer: 40 px high
    //--------------------------------------------------------------------------

    LayoutComponentStyle footer;
    footer.element.padding = {8, 8, 4, 4};

    footer.element.sizing.width.type = LayoutSizingType::Grow;

    footer.element.sizing.height.type = LayoutSizingType::Fixed;
    footer.element.sizing.height.min_value = 40;
    footer.element.sizing.height.max_value = 40;

    styles.defineComponentStyle("footer", footer);


    //--------------------------------------------------------------------------
    //  Left panel components
    //--------------------------------------------------------------------------

    LayoutComponentStyle panel_label;
    panel_label.element.padding = {4, 4, 2, 2};

    panel_label.element.sizing.width.type = LayoutSizingType::Grow;
    panel_label.element.sizing.height.min_value = 26;

    styles.defineComponentStyle("panel-label", panel_label);


    //--------------------------------------------------------------------------
    //  Right panel label
    //--------------------------------------------------------------------------

    LayoutComponentStyle right_label;
    right_label.element.padding = {4, 4, 4, 4};

    styles.defineComponentStyle("right-label", right_label);
}

} // namespace Grain::Styles


//------------------------------------------------------------------------------
//  DrawingView
//------------------------------------------------------------------------------

class DrawingView : public View {
public:

    DrawingView() = default;

    void drawContent(
        GraphicContext& gc,
        const Rectd& dirty_rect
    ) override {

        (void)dirty_rect;

        const auto& rect = bounds();

        gc.save();
        gc.clip(rect);

        // Draw a frame around the view.
        gc.setStrokeColor(Color::fromRGBA32(0x333333FF));
        gc.setStrokeWidth(20.0);
        gc.strokeRect(rect);

        // Draw a circle in the center.
        const Vec2d center{
            rect.x + rect.width * 0.5,
            rect.y + rect.height * 0.5
        };

        double radius = std::min(rect.width, rect.height) * 30.2;

        while (radius > 2) {
            gc.setFillColor(Color::fromRGBA32(0x42A5F5FF));
            gc.fillCircle(center, radius);

            gc.setStrokeColor(Color::fromRGBA32(0x1565C0FF));
            gc.setStrokeWidth(1);
            gc.strokeCircle(center, radius);

            radius *= 0.97;
        }

        gc.restore();
    }
};


class TextWordLayoutView : public View {
public:

    TextWordLayoutView();
    ~TextWordLayoutView() override = default;

    void drawContent(
        GraphicContext& gc,
        const Rectd& dirty_rect
    ) override;

    void onMouseEvent(const Event& event) override;

protected:

    enum class DragMode : uint8_t {
        None,
        Move,
        Resize
    };

    static constexpr double handle_radius_ = 6.0;
    static constexpr double min_width_ = 80.0;
    static constexpr double min_height_ = 40.0;

    String text_;

    std::unique_ptr<Font> font_{};
    TextWordLayout layout_;

    Rectd rect_ = {20, 20, 640, 480};

    DragMode drag_mode_ = DragMode::None;
    Vec2d drag_offset_{};

    [[nodiscard]]
    auto topLeftHandle() const -> Vec2d;

    [[nodiscard]]
    auto bottomRightHandle() const -> Vec2d;

    [[nodiscard]]
    auto handleContains(
        const Vec2d& handle,
        const Vec2d& position
    ) const -> bool;

    void moveRectangle(const Vec2d& position);
    void resizeRectangle(const Vec2d& position);
};


TextWordLayoutView::TextWordLayoutView() {

    font_ = std::make_unique<Font>("Geomanist Ultra", 100);

    layout_.setFont(font_.get());
    layout_.setText("The Bubble Exists Straight Behind You\u00A0II");
}


auto TextWordLayoutView::topLeftHandle() const -> Vec2d {
    return rect_.topLeft();
}


auto TextWordLayoutView::bottomRightHandle() const -> Vec2d {
    return {
        rect_.x + rect_.width,
        rect_.y + rect_.height
    };
}


auto TextWordLayoutView::handleContains(
    const Vec2d& handle,
    const Vec2d& position
) const -> bool {

    return handle.distance(position) <= handle_radius_;
}


void TextWordLayoutView::drawContent(
    GraphicContext& gc,
    const Rectd& dirty_rect
) {

    (void)dirty_rect;

    const Color text_color  = {0.05f, 0.0f, 0.35f, 1.0f};
    const Color shadow_color = {1.0f, 0.2f, 0.6f, 0.6f};

    gc.save();

    // Background
    gc.setFillColor(Color::white());
    gc.fillRect({0, 0, bounds().width, bounds().height});

    // Text layout
    layout_.layout(rect_.width, rect_.height);

    const auto& result = layout_.result();
    const double line_height = font_->lineHeight();

    for (int32_t i = 0; i < result.lineCount(); ++i) {

        const auto* line = result.line(i);

        if (line == nullptr) {
            continue;
        }

        for (int32_t j = 0; j < line->wordCount; ++j) {

            const int32_t word_index = line->firstWord + j;
            const auto* word = layout_.word(word_index);

            if (word == nullptr) {
                continue;
            }

            const Vec2d position{
                rect_.x + word->x,
                rect_.y + word->y + line_height
            };

            const String text(word->text);

            // Shadow.
            gc.setBlendMode(ColorBlendMode::Multiply);
            gc.drawText(
                text,
                position + Vec2d(6, 12),
                font_.get(),
                shadow_color
            );

            // Main text.
            gc.setBlendMode(ColorBlendMode::Multiply);
            gc.drawText(
                text,
                position,
                font_.get(),
                text_color
            );

            gc.setBlendMode(ColorBlendMode::Normal);
        }
    }

    // Rectangle outline
    gc.setStrokeColor({0.0f, 0.0f, 0.0f, 0.3f});
    gc.setStrokeWidth(1.0);
    gc.strokeRect(rect_);


    // Draw handles
    const Vec2d top_left = rect_.topLeft();

    const Vec2d bottom_right{
        rect_.x + rect_.width,
        rect_.y + rect_.height
    };

    gc.setFillColor({1.0f, 0.0f, 0.0f, 0.5f});
    gc.fillCircle(top_left, 5.0);

    gc.setFillColor({0.0f, 0.4f, 1.0f, 0.5f});
    gc.fillCircle(bottom_right, 5.0);

    // Coordinate labels.
    gc.setBlendMode(ColorBlendMode::Normal);

    gc.drawText(
        "(" + std::to_string(static_cast<int>(top_left.x)) +
        ", " + std::to_string(static_cast<int>(top_left.y)) + ")",
        top_left + Vec2d(12, -12),
        font_.get(),
        Color::black()
    );

    gc.drawText(
        "(" + std::to_string(static_cast<int>(bottom_right.x)) +
        ", " + std::to_string(static_cast<int>(bottom_right.y)) + ")",
        bottom_right + Vec2d(12, 24),
        font_.get(),
        Color::black()
    );

    gc.restore();
}


void TextWordLayoutView::moveRectangle(const Vec2d& position) {

    rect_.setPosition(position - drag_offset_);

    requestRedraw();
}


void TextWordLayoutView::resizeRectangle(const Vec2d& position) {

    const Vec2d top_left = rect_.topLeft();

    const double width = std::max(
        min_width_,
        position.x - top_left.x
    );

    const double height = std::max(
        min_height_,
        position.y - top_left.y
    );

    rect_.width = width;
    rect_.height = height;

    requestRedraw();
}


void TextWordLayoutView::onMouseEvent(const Event& event) {

    switch (event.type_) {

        case EventType::MouseButtonDown: {

            if (event.mouse_button_ != MouseButton::Left) {
                break;
            }

            const Vec2d position = event.mouse_pos_;

            // Check resize handle first.
            if (handleContains(bottomRightHandle(), position)) {

                drag_mode_ = DragMode::Resize;
                break;
            }

            // Check move handle.
            if (handleContains(topLeftHandle(), position)) {

                drag_mode_ = DragMode::Move;

                drag_offset_ = position - topLeftHandle();
                break;
            }

            break;
        }

        case EventType::MouseButtonUp: {

            if (event.mouse_button_ == MouseButton::Left) {
                drag_mode_ = DragMode::None;
            }

            break;
        }

        case EventType::MouseMove: {

            switch (drag_mode_) {

                case DragMode::Move:
                    moveRectangle(event.mouse_pos_);
                    break;

                case DragMode::Resize:
                    resizeRectangle(event.mouse_pos_);
                    break;

                case DragMode::None:
                    break;
            }

            break;
        }

        default:
            break;
    }
}


//------------------------------------------------------------------------------
//  Main
//------------------------------------------------------------------------------

int main() {

    UIStyles styles;
    Styles::create(styles);


    //--------------------------------------------------------------------------
    //  Initialize app and window
    //--------------------------------------------------------------------------

    auto& app = App::instance();

    auto window = app.addWindow(
        "layout_demo",
        "Grain Layout Demo",
        {0, 0, 1200, 800}
    );


    //--------------------------------------------------------------------------
    //  Root
    //--------------------------------------------------------------------------

    auto& root = window->rootView();
    root.setStyle(styles.viewStyle("root"));


    //--------------------------------------------------------------------------
    //  Content row
    //--------------------------------------------------------------------------

    auto& content = root.addChild<View>();
    content.setStyle(styles.viewStyle("content"));


    //--------------------------------------------------------------------------
    //  Left panel
    //--------------------------------------------------------------------------

    auto& left = content.addChild<View>();
    left.setStyle(styles.viewStyle("left-panel"));

    Random random;

    for (int32_t i = 1; i <= 30; ++i) {

        if (random.chance(0.7)) {

            left.addChild<Label>(
                "Left label " + std::to_string(i)
            ).setStyle(
                styles.componentStyle("panel-label")
            );

        }
        else if (random.chance(0.3)) {

            auto& button = left.addChild<Button>(
                "Left button " + std::to_string(i)
            );
            button.setStyle(
                styles.componentStyle("panel-label")
            );

            button.setAction([i]() {
                std::cout << "Button " << i << " clicked!" << std::endl;
            });
        }
        else {

            left.addChild<TextField>()
                .setStyle(
                    styles.componentStyle("panel-label")
                );
        }
    }


    //--------------------------------------------------------------------------
    //  Center
    //--------------------------------------------------------------------------

    auto& drawing_view = content.addChild<TextWordLayoutView>();

    drawing_view.setStyle(
        styles.viewStyle("center-view")
    );


    //--------------------------------------------------------------------------
    //  Right panel
    //--------------------------------------------------------------------------

    auto& right = content.addChild<View>();
    right.setStyle(styles.viewStyle("right-panel"));

    right.addChild<Label>("Right")
        .setStyle(styles.componentStyle("right-label"));


    //--------------------------------------------------------------------------
    //  Footer
    //--------------------------------------------------------------------------

    root.addChild<Label>("Footer")
        .setStyle(styles.componentStyle("footer"));


    //--------------------------------------------------------------------------
    //  Layout and run
    //--------------------------------------------------------------------------

    window->updateLayout();
    window->show();

    app.run();

    return 0;
}
