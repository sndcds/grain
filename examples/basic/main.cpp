#include <Grain.hpp>

#include <cstdint>
#include <iostream>


class TestView : public Grain::View {
public:
    void draw(Grain::Canvas& canvas) override {
        std::cout << "Draw view\n";
        canvas.clear({
            0.1f,
            0.1f,
            0.1f,
            1.0f
        });

        canvas.fillRect(
            {
                100.0,
                100.0,
                300.0,
                200.0
            },
            Grain::Color::redColor()
            );
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