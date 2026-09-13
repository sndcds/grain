#include <Grain.hpp>

#include <cstdint>
#include <iostream>

constexpr double k = 0.5522847498307936;


class TestView : public Grain::View {
public:
    void draw(Grain::GraphicContext& gc) override {
        gc.rotateDegrees(0);
        gc.setFillColor({1, .8, .6, 1});
        gc.fillRect({20.0, 20.0, 800.0 - 40.0, 600.0 - 40.0});

        gc.setStrokeColor({0, 0, 0, 1});

        Grain::GraphicPath path;
        path.addPoint({20.0, 0.0}, {20.0, -10.0}, {20.0, 10.0});
        path.addPoint({0.0, 20.0}, {10.0, 20.0}, {-10.0, 20.0});
        path.addPoint({-20.0, 0.0}, {-20.0, 10.0}, {-20.0, -10.0});
        path.addPoint({0.0, 0.0}, {-10.0, -20.0}, {10.0, -20.0});
        path.setClosed(true);

        for (double y = 0; y < 1280; y += 45) {
            for (double x = 0; x < 1920; x += 45) {
                gc.save();

                gc.translate(x + std::sin(y / 100) * 30, y);

                gc.setFillColor({.2, .8, .4, 1});
                gc.fillPath(path);

                gc.setStrokeColor(Grain::Color::black());
                gc.setLineWidth(3.0);
                gc.strokePath(path);

                gc.restore();
            }
        }
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

    GraphicPath path;

    path.addPoint(
        10.0, 10.0,
        20.0, 10.0,
        10.0, 20.0
    );

    path.rotatePointDegrees(0, 90.0);

    const auto* point =
        path.pointPtrAtIndex(0);

    std::cout << "point->anchor,: " << point->anchor << " ... " << Vec2d{10.0, 10.0} << std::endl;
    std::cout << "point->left,: " << point->left << " ... " << Vec2d{10.0, 20.0} << std::endl;
    std::cout << "point->right,: " << point->anchor << " ... " << Vec2d{0.0, 10.0} << std::endl;

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