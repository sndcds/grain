#include <Grain.hpp>

#include <cstdint>
#include <iostream>


class TestView : public Grain::View {
public:
    void draw(Grain::GraphicContext& gc) override {
        gc.rotateDegrees(10);
        gc.setFillColor({1, .8, .6, 1});
        gc.fillRect({20.0, 20.0, 800.0 - 40.0, 600.0 - 40.0});
    }
};


void stringExample() {

    using Grain::String;

    String title = "  Sommer in Flensburg 🌊  ";

    // Check the input.
    if (!title.isValidUtf8()) {
        return;
    }

    // Remove surrounding whitespace.
    title.trim();

    // Inspect the result.
    if (title.empty()) {
        return;
    }

    // Check whether it contains a particular word.
    if (title.contains("Flensburg")) {
        // ...
    }

    // Find a word. The result is a code-point index.
    auto position = title.find("Flensburg");

    if (position != String::npos) {
        String location = title.substr(position);

        std::cout << "Location: " << location << '\n';
    }

    // Append additional text.
    title += " – Kultur";

    // Compare the result.
    if (title.startsWith("Sommer")) {
        std::cout << title << '\n';
    }
}

int main() {

    using namespace Grain;

    const Bezier bezier{
        Vec2d{0.0, 0.0},
        Vec2d{10.0, 0.0},
        Vec2d{10.0, 10.0},
        Vec2d{20.0, 10.0}
    };

    Bezier result;

    bool truncateOk = bezier.truncate(0.25, 0.75, result);
    auto startPos = bezier.posOnCurve(0.25);
    auto endPos = bezier.posOnCurve(0.75);

    std::cout << result.startPos() << " ... " << startPos << std::endl;
    std::cout << result.endPos() << " ... " << endPos << std::endl;


    auto& app = Grain::App::instance();

    stringExample();

    auto* window = app.createWindow("Grain", 800, 600);
    auto view = std::make_unique<TestView>();
    view->setBounds({0.0, 0.0, 800.0, 600.0});
    window->setRootView(std::move(view));

    {
        auto* window2 = app.createWindow("Grain2", 800, 600);
        auto view2 = std::make_unique<TestView>();
        view2->setBounds({0.0, 0.0, 800.0, 600.0});
        window2->setRootView(std::move(view2));
    }

    app.run();

    return 0;
}