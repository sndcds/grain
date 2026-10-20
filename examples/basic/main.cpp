#include "grain/UI/TextField.hpp"

#include <Grain.hpp>

#include <cstdint>
#include <iomanip>
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

    void drawContent(Grain::GraphicContext& gc, const Grain::Rectd& dirty_rect) override {

        gc.save();

        Grain::Gradient gradient;
        gradient.addStop(0, {1, 0, 0, 1});
        gradient.addStop(.2, {1, 1, 0, 1});
        gradient.addStop(.5, {0, .5, 1, 1});
        gradient.addStop(1, {0, 1, .5, 1});

        gc.setFillColor({.96, .96, .96, 1});
        gc.fillRect(dirty_rect);

        gc.addRingPath(dirty_rect.center(), dirty_rect.shortSide() / 2, dirty_rect.shortSide() / 4);
        // gc.setFillColor({1, 0, 0, 1});
        // gc.fillPathEvenOdd();
        gc.clipPathEvenOdd();
        gc.fillGradient(&gradient, {.0, .0}, {1000, 1000}, true, true);


        /*
        for (int32_t i = 0; i < 100; i++) {
            gc.save();
            gc.translate(animationX_, i);
            gc.rotateDegrees(rotation_ * (90.0 + i));
            gc.setFillColor(Grain::Color(1, 1.0 / 100 * i, 0.2, .03));
            gc.fillRect({-1600, 100, 1600 * 2, 300});
            gc.restore();
        }
        */
        gc.restore();
    }

    void handleEvent(const Grain::Event& event) override {
        View::handleEvent(event);
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
    Grain::Random random;

    float value = random.real();
    float offset = random.bipolar(10.0f);
    int32_t index = random.integer(0, 99);
    bool enabled = random.chance(0.75f);
    float noise = random.gaussian();


    auto& app = App::instance();
    app.setIcon("/Users/roaldchristesen/Documents/Pippa/Pippa Logo.png");

    auto *window = app.createWindow("Grain", 400,  400);

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
                            0.2,
                            [&button, x, y](Grain::Animation& animation) -> void {
                                const double progress = animation.progress();

                                Rectd src = {x, y, 120.0, 30.0};
                                Rectd dst = {x - 20, y - 10, 120 + 40.0, 50.0};
                                button.setBounds(src.lerp(dst, progress));
                            });
                    });
                }
                i++;
            }
        }

        Random random;

        for (double y = 20; y < 1280; y += 40) {
            auto& slider = view->addComponent<Slider>(0.0f, 100.0f, 50.0f);

            slider.setBounds(Rectd{20.0, y, 240.0, 30.0});
            slider.setStep(1.0f);

            slider.setOrientation(Slider::Orientation::Horizontal);
            slider.setTrackSize(random.real(2, 12));

            // slider.setTrackColor(Color{random.real(), random.real(), random.real(), 1.0f});
            // slider.setIndicatorColor(Color{random.real(), random.real(), random.real(), 1.0f});
            // slider.setHandleColor(Color{random.real(), random.real(), random.real(), 1.0f});

            slider.setAction([](double value) -> void {
            });
        }

        for (double x = 300; x < 1280; x += 140) {
            auto& slider = view->addComponent<Slider>(0.0f, 100.0f, 50.0f);

            slider.setBounds(Rectd{x, 200, 120, 800.0});
            slider.setStep(0.01f);

            slider.setOrientation(Slider::Orientation::Vertical);
            slider.setTrackSize(random.real(20, 120));
            auto s = random.real(slider.trackSize(), slider.trackSize() * 2);
            slider.setHandleSize({s, s});

            slider.setTrackColor(Color{random.real(), random.real(), random.real(), 1.0f});
            slider.setIndicatorColor(Color{random.real(), random.real(), random.real(), 1.0f});
            slider.setHandleColor(Color{random.real(), random.real(), random.real(), 1.0f});

            slider.setAction([](double value) -> void {
            });
        }

        {
            auto& text_field = view->addComponent<TextField>();
            text_field.setBounds(Rectd{500, 500, 240, 40.0});
            text_field.setText("Hello World! 1234567890");
            text_field.setFocused(true);
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