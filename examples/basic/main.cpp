#include <Grain.hpp>

#include <cstdint>
#include <iostream>


class TestView : public Grain::View {
public:
    Grain::Vec2d mouse_pos_{};

    double animationX_ = 0.0;
    double rotation_ = 0.0;

    void startAnimation() {
        auto& driver = Grain::AnimationDriver::instance();

        driver.start();

        driver.animate(
            0.0,
            30.0,
            [this](Grain::Animation& animation) {
                animationX_ = animation.progress() * 1200.0;
                requestRedraw();
            });

        driver.animate(
            0.0,
            30.0,
            [this](Grain::Animation& animation) {
                rotation_ = animation.progress();
                requestRedraw();
            });
    }

    void drawContent(Grain::GraphicContext& gc) override {
        gc.setFillColor({.96, .96, .96, 1});
        gc.fillRect({0, 0, 2000, 1300});

        for (int32_t i = 0; i < 100; i++) {
            gc.save();
            gc.translate(animationX_, i);
            gc.rotateDegrees(rotation_ * (90.0 + i));
            gc.setFillColor(Grain::Color(1, 1.0 / 100 * i, 0.2, .03));
            gc.fillRect({-1600, 100, 1600 * 2, 300});
            gc.restore();
        }
    }

    void handleEvent(const Grain::Event& event) override {
        if (event.type_ == Grain::EventType::KeyDown) {
            std::cout << "KeyDown" << std::endl;
            startAnimation();
        }
        else {
            View::handleEvent(event);
        }

        mouse_pos_ = event.mouse_pos_;

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
    app.setIcon("/Users/roaldchristesen/Documents/Pippa/Pippa Logo.png");

    auto window = app.createWindow("Grain", 400,  400);

    {
        auto view = std::make_unique<TestView>();

        int i = 0;
        for (double y = 20; y < 1280; y += 50) {
            for (double x = 20; x < 1920; x += 160) {

                if ((i % 7) == 0) {
                    auto& label = view->addComponent<Label>("Hello");
                    label.setBounds({x, y, 120.0, 30.0});
                }
                else {
                    auto& button = view->addComponent<Button>("Hello");
                    button.setBounds({x, y, 120.0, 30.0});

                    button.setAction([&button, x, y] -> void {
                        auto& driver = Grain::AnimationDriver::instance();

                        driver.start();

                        driver.animate(
                            0.0,
                            1.0,
                            [&button, x, y](Grain::Animation& animation) -> void {
                                const double progress = animation.progress();

                                button.setBounds({
                                    x + 100.0 * progress,
                                    y,
                                    120.0,
                                    30.0
                                });
                            });
                    });
                }
                i++;
            }
        }

        window->setRootView(std::move(view));
    }

    auto families = Grain::Platform::availableFontNames();
    for (const auto& family : families) {
        std::cout << family << '\n';
    }

    Font font("ThisFontDefinitelyDoesNotExist_12345", 24.0f);
    std::cout << "font fontName: " << font.fontNameUtf8() << '\n';
    std::cout << "font displayName: " << font.displayNameUtf8() << '\n';
    std::cout << "font size: " << font.size() << '\n';

    app.run();

    stringExample();
}