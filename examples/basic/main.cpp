#include <Grain.hpp>

#include <cstdint>
#include <iostream>


class TestView : public Grain::View {
public:
    void draw(Grain::GraphicContext& gc) override {
        std::cout << "Draw view\n";

        gc.setFillColor(Grain::Color::redColor());

        gc.fillRect({
            100.0,
            100.0,
            300.0,
            200.0
        });
    }
};


int main() {
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

    auto* window = app.createWindow(
        "Grain",
        800,
        600
        );

    auto view = std::make_unique<TestView>();

    view->setBounds({
        0.0,
        0.0,
        800.0,
        600.0
    });

    window->setRootView(std::move(view));

    app.run();

    return 0;
}