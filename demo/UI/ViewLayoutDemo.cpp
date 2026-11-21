#include "grain/UI/TextField.hpp"

#include <Grain.hpp>

#include <cstdint>
#include <iomanip>
#include <iostream>


using namespace Grain;

int main() {

    // Initialize the app
    auto& app = App::instance();
    app.setIcon("/Users/roaldchristesen/Documents/Pippa/Pippa Logo.png");

    auto window = app.addWindow("Grain", {0, 0, 640, 480});

    auto& view = window->rootView();

    auto& label = view.addChild<Label>("Hello");
	label.setBounds({100, 100, 200, 32});


	window->updateLayout();

    app.run();
}