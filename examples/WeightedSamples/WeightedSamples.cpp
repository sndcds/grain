#include <Grain.hpp>

#include <algorithm>
#include <iomanip>
#include <iostream>

int main() {
    using Grain::WeightedSamples;

    constexpr int32_t kResolution = 33;

    WeightedSamples curve(kResolution);

    if (curve.lastError() != WeightedSamples::kErrNone) {
        std::cerr << "Could not create WeightedSamples. Error: "
            << static_cast<int>(curve.lastError()) << '\n';
        return 1;
    }

    // Define a few control values. Values between these points are handled
    // By WeightedSamples::lookup().
    curve.setSample(0, 0.1f);
    curve.setSample(4, 0.35f);
    curve.setSample(10, 10.85f);
    curve.setSample(16, 0.45f);
    curve.setSample(23, 0.70f);
    curve.setSample(kResolution - 1, -1.2f);

    // Give each control sample a weight. A weight of 1 means normal influence.
    // Adjust these values to see how your implementation handles weighting.
    curve.setWeight(0, 1.0f);
    curve.setWeight(4, 1.0f);
    curve.setWeight(10, 1.5f);
    curve.setWeight(16, 0.8f);
    curve.setWeight(23, 1.2f);
    curve.setWeight(kResolution - 1, 1.0f);

    curve.finish();

    float min_y = 0.0f;
    float max_y = 0.0f;
    curve.minMaxY(&min_y, &max_y);

    std::cout << "Grain::WeightedSamples demo\n"
        << "Resolution: " << curve.resolution() << '\n'
        << "Min Y:      " << min_y << '\n'
        << "Max Y:      " << max_y << "\n\n";

    // Draw a compact ASCII graph. This assumes lookup(t) uses t in [0, 1].
    constexpr int kColumns = 65;
    constexpr int kRows = 16;

    float values[kColumns]{};
    for (int x = 0; x < kColumns; ++x) {
        const float t = static_cast<float>(x) / static_cast<float>(kColumns - 1);
        values[x] = curve.lookup(t);
    }

    const float graph_min = std::min(0.0f, min_y);
    const float graph_max = std::max(1.0f, max_y);
    const float graph_range = graph_max - graph_min;

    for (int row = kRows - 1; row >= 0; --row) {
        const float y = graph_min + graph_range *
                        static_cast<float>(row) / static_cast<float>(kRows - 1);

        std::cout << std::fixed << std::setprecision(2) << std::setw(5) << y
            << " |";

        for (int x = 0; x < kColumns; ++x) {
            const int sample_row = static_cast<int>(
                std::round((values[x] - graph_min) / graph_range * (kRows - 1)));

            std::cout << (sample_row == row ? '*' : ' ');
        }
        std::cout << '\n';
    }

    std::cout << "      +" << std::string(kColumns, '-') << '\n'
        << "       0" << std::string(kColumns - 8, ' ') << "1  (t)\n\n";

    std::cout << "Lookup values:\n";
    for (int i = 0; i <= 10; ++i) {
        const float t = static_cast<float>(i) / 10.0f;
        std::cout << "  lookup(" << std::fixed << std::setprecision(2) << t
            << ") = " << std::setprecision(4) << curve.lookup(t) << '\n';
    }


    auto& app = Grain::App::instance();
    app.setIcon("/Users/roaldchristesen/Documents/Pippa/Pippa Logo.png");

    auto *window = app.createWindow("Grain", 400,  400);
    app.run();

    return 0;
}