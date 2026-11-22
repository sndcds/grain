#include <Grain.hpp>

#include <cstdint>
#include <iomanip>
#include <iostream>


using namespace Grain;


namespace Grain::Styles {

inline auto create(UIStyles& styles) -> void {

    LayoutViewStyle vertical_stack;

    vertical_stack.direction = LayoutDirection::TopToBottom;
    vertical_stack.child_horizontal_alignment = LayoutAlignment::Start;
    vertical_stack.child_vertical_alignment = LayoutAlignment::Start;

    vertical_stack.element.padding = {10.0, 10.0, 10.0, 10.0};
    vertical_stack.element.child_gap = 10.0;
    vertical_stack.element.sizing.width.min_value = 100;
    vertical_stack.element.sizing.height.min_value = 100;

    styles.defineViewStyle("vertical-stack", vertical_stack);


    LayoutViewStyle horizontal_stack;

    horizontal_stack.direction = LayoutDirection::LeftToRight;
    horizontal_stack.element.child_gap = 10.0;
    horizontal_stack.element.padding = {10.0, 10.0, 10.0, 10.0};
    horizontal_stack.element.sizing.width.type = LayoutSizingType::Grow;

    styles.defineViewStyle("horizontal-stack", horizontal_stack);


    LayoutComponentStyle default_component;
    default_component.element.padding = {10.0, 10.0, 10.0, 10.0};
    default_component.element.child_gap = 10.0;
    default_component.element.sizing.width.min_value = 100;


    styles.defineComponentStyle(
        "default",
        default_component
    );
}

}


int main() {

	UIStyles styles;


    //--------------------------------------------------------------------------
    //  Initialize the app
    //--------------------------------------------------------------------------

    auto& app = App::instance();
    app.setIcon("/Users/roaldchristesen/Documents/Pippa/Pippa Logo.png");

	Styles::create(styles);

    auto window = app.addWindow("test_window", "Grain", {0, 0, 1600, 800});

    auto& view = window->rootView();
	view.setStyle(styles.viewStyle("horizontal-stack"));

    auto& label = view.addChild<Label>("Hello");
	label.setStyle(styles.componentStyle("default"));

    auto& sub_view = view.addChild<View>();
	sub_view.setStyle(styles.viewStyle("vertical-stack"));

    sub_view.addChild<Label>("X-100")
		.setStyle(styles.componentStyle("default"));

    view.addChild<Label>("Synthesizerstudio")
		.setStyle(styles.componentStyle("default"));
    view.addChild<Label>("Apple")
		.setStyle(styles.componentStyle("default"));
    view.addChild<Label>("Silicon Graphics")
		.setStyle(styles.componentStyle("default"));
    view.addChild<Label>("Digital Equipment")
		.setStyle(styles.componentStyle("default"));
    view.addChild<Label>("Atari")
		.setStyle(styles.componentStyle("default"));
    view.addChild<Label>("Commodore")
		.setStyle(styles.componentStyle("default"));
    view.addChild<Label>("IBM")
		.setStyle(styles.componentStyle("default"));


	window->updateLayout();
    window->show();

    app.run();
}