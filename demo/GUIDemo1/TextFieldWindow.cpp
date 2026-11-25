#include "TextFieldWindow.hpp"

using namespace Grain;


//------------------------------------------------------------------------------
//  Styles
//------------------------------------------------------------------------------

namespace TextFieldStyles {

inline auto create(UIStyles& styles) -> void {

    //--------------------------------------------------------------------------
    //  Root: vertical layout
    //--------------------------------------------------------------------------

    LayoutViewStyle root;
    root.direction = LayoutDirection::TopToBottom;
    root.element.padding = {0, 0, 0, 0};
    root.element.child_gap = 0;

    root.element.sizing.width.type = LayoutSizingType::Grow;
    root.element.sizing.height.type = LayoutSizingType::Grow;

    styles.defineViewStyle("root", root);


    //--------------------------------------------------------------------------
    //  Spacer: grows in both directions
    //--------------------------------------------------------------------------

    LayoutViewStyle spacer;
    spacer.direction = LayoutDirection::TopToBottom;

    spacer.element.sizing.width.type = LayoutSizingType::Grow;
    spacer.element.sizing.height.type = LayoutSizingType::Grow;

    styles.defineViewStyle("spacer", spacer);


    //--------------------------------------------------------------------------
    //  Row: fixed height, fills available width
    //--------------------------------------------------------------------------

    LayoutViewStyle row;
    row.direction = LayoutDirection::LeftToRight;

    row.element.padding = {0, 0, 0, 0};
    row.element.child_gap = 0;

    row.element.sizing.width.type = LayoutSizingType::Grow;

    row.element.sizing.height.type = LayoutSizingType::Fixed;
    row.element.sizing.height.min_value = 48;
    row.element.sizing.height.max_value = 48;

    styles.defineViewStyle("row", row);


    //--------------------------------------------------------------------------
    //  Text field: fixed size
    //--------------------------------------------------------------------------

    LayoutComponentStyle text_field;
    text_field.element.sizing.width.type = LayoutSizingType::Fixed;
    text_field.element.sizing.width.min_value = 320;
    text_field.element.sizing.width.max_value = 320;

    text_field.element.sizing.height.type = LayoutSizingType::Fixed;
    text_field.element.sizing.height.min_value = 40;
    text_field.element.sizing.height.max_value = 40;

    styles.defineComponentStyle("text-field", text_field);
}

} // namespace TextFieldStyles


//------------------------------------------------------------------------------
//  TextFieldWindow
//------------------------------------------------------------------------------

namespace Demo {

TextFieldWindow::TextFieldWindow(App& app) {

    TextFieldStyles::create(styles_);

    window_ = app.addWindow(
        "textfield_demo",
        "Grain TextField Demo",
        {100, 100, 800, 600}
    );

    auto& root = window_->rootView();
    root.setStyle(styles_.viewStyle("root"));


    //--------------------------------------------------------------------------
    //  Top spacer
    //--------------------------------------------------------------------------

    root.addChild<View>().setStyle(
        styles_.viewStyle("spacer")
    );


    //--------------------------------------------------------------------------
    //  First row
    //--------------------------------------------------------------------------

    auto& first_row = root.addChild<View>();
    first_row.setStyle(styles_.viewStyle("row"));

    first_row.addChild<View>().setStyle(
        styles_.viewStyle("spacer")
    );

    first_row.addChild<TextField>().setStyle(
        styles_.componentStyle("text-field")
    );

    first_row.addChild<View>().setStyle(
        styles_.viewStyle("spacer")
    );


    //--------------------------------------------------------------------------
    //  Second row
    //--------------------------------------------------------------------------

    auto& second_row = root.addChild<View>();
    second_row.setStyle(styles_.viewStyle("row"));

    second_row.addChild<View>().setStyle(
        styles_.viewStyle("spacer")
    );

    second_row.addChild<TextField>().setStyle(
        styles_.componentStyle("text-field")
    );

    second_row.addChild<View>().setStyle(
        styles_.viewStyle("spacer")
    );


    //--------------------------------------------------------------------------
    //  Bottom spacer
    //--------------------------------------------------------------------------

    root.addChild<View>().setStyle(
        styles_.viewStyle("spacer")
    );


    //--------------------------------------------------------------------------
    //  Layout and show
    //--------------------------------------------------------------------------

    window_->updateLayout();
    window_->show();
}

} // namespace Demo