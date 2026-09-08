#include <Grain.hpp>

#include <cstdint>
#include <iostream>


class TestView : public Grain::View {
public:
    void draw(Grain::GraphicContext& gc) override {
        gc.setFillColor(Grain::Color::redColor());
        gc.fillRect({100.0, 100.0, 200.0, 300.0});
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

    stringExample();

    struct Point {
        int32_t a, b, c;
        double d;
    };

    Grain::ObjectBase object;
    Grain::List<Point> points;

    std::cout << "Object ID: " << object.id() << '\n';

    points.push_back(Point());
    points.push_back(Point());
    points.push_back(Point());

    std::cout << "Points size: " << points.size() << '\n';

    auto& app = Grain::App::instance();

    auto* window = app.createWindow("Grain", 800, 600);
    auto view = std::make_unique<TestView>();
    view->setBounds({0.0, 0.0, 800.0, 600.0});
    window->setRootView(std::move(view));

    {
        auto* window2 = app.createWindow("Grain2", 900, 700);
        auto view2 = std::make_unique<TestView>();
        view2->setBounds({0.0, 0.0, 800.0, 600.0});
        window2->setRootView(std::move(view2));
    }

    app.run();

    return 0;
}