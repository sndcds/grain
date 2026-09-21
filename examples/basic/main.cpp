#include <Grain.hpp>

#include <cstdint>
#include <iostream>

void drawButton(Grain::GraphicContext& gc, const Grain::String& text, const Grain::Rectd& bounds, const Grain::Font& font) {
    double radius = 14;
    gc.setFillColor({1, 1, 1, 1});
    gc.addRoundRectPath(bounds, radius);
    gc.fillPath();
    // gc.setStrokeSize(1);
    auto b = bounds;
    b.inset(0.5);
    gc.addRoundRectPath(b, radius);
    gc.setStrokeColor({0, 0, 0, 1});
    gc.strokePath();
    // gc.strokeRect({bounds.x + 0.5, bounds.y + 0.5, bounds.width - 1, bounds.height - 1});
    gc.drawTextInRect(text, bounds, Grain::Alignment::Center, &font, {0, 0, 0, 1});
}


class TestView : public Grain::View {
public:
    double mx_{};
    double my_{};

    void draw(Grain::GraphicContext& gc) override {
        constexpr double k = 0.5522847498307936;
        /*

        gc.rotateDegrees(0);
        gc.setFillColor({0, 0, 0, 1});
        gc.fillRect({0, 0, 2000, 1300});

        gc.setStrokeColor({0, 0, 0, 1});

        Grain::GraphicPath path;
        path.addPoint({20.0, 0.0}, {20.0, -10.0}, {20.0, 10.0});
        path.addPoint({0.0, 20.0}, {10.0, 20.0}, {-10.0, 20.0});
        path.addPoint({-20.0, 0.0}, {-20.0, 10.0}, {-20.0, -10.0});
        path.addPoint({0.0, 0.0}, {-10.0, -20.0}, {10.0, -20.0});
        path.setClosed(true);

        for (double y = 0; y < 1280; y += 50) {
            for (double x = 0; x < 1920; x += 50) {
                gc.save();

                gc.translate(x + std::sin(y / 100) * my_, y + std::sin(x / 100) * mx_);
                gc.scale(3, 3);
                gc.addPath(path);

                gc.setFillColor({.2, .8, .4, .1});
                gc.fillPath(path);

                gc.setStrokeColor(Grain::Color::black());
                gc.setLineWidth(3.0);
                gc.strokePath(path);

                gc.restore();
            }
        }
        */

        Grain::Font font("Jetbrains", 28.0f);
        for (double y = 0; y < 1280; y += 200) {
            for (double x = 0; x < 1920; x += 300) {
                gc.save();
                gc.translate(std::sin(y / 100) * my_, std::sin(x / 100) * mx_);
                drawButton(gc, "Compile", {x, y, 160, 34}, font);
                gc.restore();
            }
        }
    }

    void handleEvent(const Grain::Event& event) override {
        if (event.type == Grain::EventType::KeyDown) {
            std::cout << "Key" << std::endl;
        }
        mx_ = event.mouseX;
        my_ = event.mouseY;
        requestRedraw();
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


using namespace Grain;

int main() {

    auto& app = App::instance();
    auto window = app.createWindow("Grain", 400,  400);
    auto view = std::make_unique<TestView>();
    window->setRootView(std::move(view));

    Font font("ThisFontDefinitelyDoesNotExist_12345", 24.0f);
    std::cout << "font fontName: " << font.fontNameUtf8() << '\n';
    std::cout << "font displayName: " << font.displayNameUtf8() << '\n';
    std::cout << "font size: " << font.size() << '\n';

    app.run();


    stringExample();
    return 0;
}