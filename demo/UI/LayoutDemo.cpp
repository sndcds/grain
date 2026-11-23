
#include <Grain.hpp>

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
    content.element.child_gap = 0;

    content.element.sizing.width.type = LayoutSizingType::Grow;
    content.element.sizing.height.type = LayoutSizingType::Grow;

    styles.defineViewStyle("content", content);


    //--------------------------------------------------------------------------
    //  Left panel: 100 px wide
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
    //  Right panel: 100 px wide
    // --------------------------------------------------

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
    //  Left panel labels
    //--------------------------------------------------------------------------

    LayoutComponentStyle panel_label;
    panel_label.element.padding = {4, 4, 2, 2};
    panel_label.element.sizing.width.type = LayoutSizingType::Grow;
    panel_label.element.sizing.height.min_value = 26;

    styles.defineComponentStyle("panel-label", panel_label);


    //--------------------------------------------------------------------------
    //  Center label: fills available space
    //--------------------------------------------------------------------------

    LayoutComponentStyle center_label;
    center_label.element.padding = {8, 8, 8, 8};

    center_label.element.sizing.width.type = LayoutSizingType::Grow;
    center_label.element.sizing.height.type = LayoutSizingType::Grow;
    center_label.element.sizing.width.min_value = 200;

    styles.defineComponentStyle("center-label", center_label);


    //--------------------------------------------------------------------------
    //  Right panel label
    //--------------------------------------------------------------------------

    LayoutComponentStyle right_label;
    right_label.element.padding = {4, 4, 4, 4};

    styles.defineComponentStyle("right-label", right_label);
}

} // namespace Grain::Styles


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
            left.addChild<Button>(
                "Left label " + std::to_string(i)
            ).setStyle(
                styles.componentStyle("panel-label")
            );
        }
        else {
            left.addChild<TextField>().setStyle(
                styles.componentStyle("panel-label")
            );
        }
    }


    //--------------------------------------------------------------------------
    //  Center
    //--------------------------------------------------------------------------

    content.addChild<Label>("Center content")
        .setStyle(styles.componentStyle("center-label"));


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
