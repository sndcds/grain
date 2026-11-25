#include <Grain.hpp>

#include "LayoutWindow.hpp"
#include "TextFieldWindow.hpp"

using namespace Grain;


int main() {

    auto& app = App::instance();

    Demo::LayoutWindow layout_demo(app);
    Demo::TextFieldWindow textfield_demo(app);

    app.run();

    return 0;
}