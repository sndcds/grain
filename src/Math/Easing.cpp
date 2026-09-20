#include <grain/Math/Easing.hpp>

#include <cmath>
#include <numbers>

namespace Grain::Easing {

namespace {

constexpr double kPi = std::numbers::pi;
constexpr double kTau = std::numbers::pi * 2.0;

constexpr double kBackC1 = 1.70158;
constexpr double kBackC2 = kBackC1 * 1.525;
constexpr double kBackC3 = kBackC1 + 1.0;

constexpr double kElasticC4 = kTau / 3.0;
constexpr double kElasticC5 = kTau / 4.5;

}

double easeInSine(const double t) noexcept {
    return 1.0 - std::cos(t * kPi / 2.0);
}

double easeOutSine(const double t) noexcept {
    return std::sin(t * kPi / 2.0);
}

double easeInOutSine(const double t) noexcept {
    return -(std::cos(kPi * t) - 1.0) / 2.0;
}


double easeInQuad(const double t) noexcept {
    return t * t;
}

double easeOutQuad(const double t) noexcept {
    return 1.0 - (1.0 - t) * (1.0 - t);
}

double easeInOutQuad(const double t) noexcept {
    if (t < 0.5) {
        return 2.0 * t * t;
    }

    return 1.0 - std::pow(-2.0 * t + 2.0, 2.0) / 2.0;
}


double easeInCubic(const double t) noexcept {
    return t * t * t;
}

double easeOutCubic(const double t) noexcept {
    return 1.0 - std::pow(1.0 - t, 3.0);
}

double easeInOutCubic(const double t) noexcept {
    if (t < 0.5) {
        return 4.0 * t * t * t;
    }

    return 1.0 - std::pow(-2.0 * t + 2.0, 3.0) / 2.0;
}


double easeInQuart(const double t) noexcept {
    return t * t * t * t;
}

double easeOutQuart(const double t) noexcept {
    return 1.0 - std::pow(1.0 - t, 4.0);
}

double easeInOutQuart(const double t) noexcept {
    if (t < 0.5) {
        return 8.0 * t * t * t * t;
    }

    return 1.0 - std::pow(-2.0 * t + 2.0, 4.0) / 2.0;
}


double easeInQuint(const double t) noexcept {
    return t * t * t * t * t;
}

double easeOutQuint(const double t) noexcept {
    return 1.0 - std::pow(1.0 - t, 5.0);
}

double easeInOutQuint(const double t) noexcept {
    if (t < 0.5) {
        return 16.0 * t * t * t * t * t;
    }

    return 1.0 - std::pow(-2.0 * t + 2.0, 5.0) / 2.0;
}


double easeInExpo(const double t) noexcept {
    if (t == 0.0) {
        return 0.0;
    }

    return std::pow(2.0, 10.0 * t - 10.0);
}

double easeOutExpo(const double t) noexcept {
    if (t == 1.0) {
        return 1.0;
    }

    return 1.0 - std::pow(2.0, -10.0 * t);
}

double easeInOutExpo(const double t) noexcept {
    if (t == 0.0 || t == 1.0) {
        return t;
    }

    if (t < 0.5) {
        return std::pow(2.0, 20.0 * t - 10.0) / 2.0;
    }

    return (2.0 - std::pow(2.0, -20.0 * t + 10.0)) / 2.0;
}


double easeInCirc(const double t) noexcept {
    return 1.0 - std::sqrt(1.0 - t * t);
}

double easeOutCirc(const double t) noexcept {
    return std::sqrt(1.0 - (t - 1.0) * (t - 1.0));
}

double easeInOutCirc(const double t) noexcept {
    if (t < 0.5) {
        return (1.0 - std::sqrt(1.0 - 4.0 * t * t)) / 2.0;
    }

    return (
        std::sqrt(
            1.0 - std::pow(-2.0 * t + 2.0, 2.0)
        ) + 1.0
    ) / 2.0;
}


double easeInBack(const double t) noexcept {
    return kBackC3 * t * t * t - kBackC1 * t * t;
}

double easeOutBack(const double t) noexcept {
    const double x = t - 1.0;

    return 1.0 + kBackC3 * x * x * x + kBackC1 * x * x;
}

double easeInOutBack(const double t) noexcept {
    if (t < 0.5) {
        const double x = 2.0 * t;

        return (
            x * x * ((kBackC2 + 1.0) * x - kBackC2)
        ) / 2.0;
    }

    const double x = 2.0 * t - 2.0;

    return (
        x * x * ((kBackC2 + 1.0) * x + kBackC2) + 2.0
    ) / 2.0;
}


double easeInElastic(const double t) noexcept {
    if (t == 0.0 || t == 1.0) {
        return t;
    }

    return -std::pow(2.0, 10.0 * t - 10.0)
        * std::sin((t * 10.0 - 10.75) * kElasticC4);
}

double easeOutElastic(const double t) noexcept {
    if (t == 0.0 || t == 1.0) {
        return t;
    }

    return std::pow(2.0, -10.0 * t)
        * std::sin((t * 10.0 - 0.75) * kElasticC4)
        + 1.0;
}

double easeInOutElastic(const double t) noexcept {
    if (t == 0.0 || t == 1.0) {
        return t;
    }

    if (t < 0.5) {
        return -(
            std::pow(2.0, 20.0 * t - 10.0)
            * std::sin((20.0 * t - 11.125) * kElasticC5)
        ) / 2.0;
    }

    return (
        std::pow(2.0, -20.0 * t + 10.0)
        * std::sin((20.0 * t - 11.125) * kElasticC5)
    ) / 2.0 + 1.0;
}


double easeOutBounce(const double t) noexcept {
    constexpr double n1 = 7.5625;
    constexpr double d1 = 2.75;

    if (t < 1.0 / d1) {
        return n1 * t * t;
    }

    if (t < 2.0 / d1) {
        const double x = t - 1.5 / d1;

        return n1 * x * x + 0.75;
    }

    if (t < 2.5 / d1) {
        const double x = t - 2.25 / d1;

        return n1 * x * x + 0.9375;
    }

    const double x = t - 2.625 / d1;

    return n1 * x * x + 0.984375;
}

double easeInBounce(const double t) noexcept {
    return 1.0 - easeOutBounce(1.0 - t);
}

double easeInOutBounce(const double t) noexcept {
    if (t < 0.5) {
        return (1.0 - easeOutBounce(1.0 - 2.0 * t)) / 2.0;
    }

    return (1.0 + easeOutBounce(2.0 * t - 1.0)) / 2.0;
}


double evaluate(const Type type, const double t) noexcept {
    switch (type) {
        case Type::InSine:
            return easeInSine(t);
        case Type::OutSine:
            return easeOutSine(t);
        case Type::InOutSine:
            return easeInOutSine(t);

        case Type::InQuad:
            return easeInQuad(t);
        case Type::OutQuad:
            return easeOutQuad(t);
        case Type::InOutQuad:
            return easeInOutQuad(t);

        case Type::InCubic:
            return easeInCubic(t);
        case Type::OutCubic:
            return easeOutCubic(t);
        case Type::InOutCubic:
            return easeInOutCubic(t);

        case Type::InQuart:
            return easeInQuart(t);
        case Type::OutQuart:
            return easeOutQuart(t);
        case Type::InOutQuart:
            return easeInOutQuart(t);

        case Type::InQuint:
            return easeInQuint(t);
        case Type::OutQuint:
            return easeOutQuint(t);
        case Type::InOutQuint:
            return easeInOutQuint(t);

        case Type::InExpo:
            return easeInExpo(t);
        case Type::OutExpo:
            return easeOutExpo(t);
        case Type::InOutExpo:
            return easeInOutExpo(t);

        case Type::InCirc:
            return easeInCirc(t);
        case Type::OutCirc:
            return easeOutCirc(t);
        case Type::InOutCirc:
            return easeInOutCirc(t);

        case Type::InBack:
            return easeInBack(t);
        case Type::OutBack:
            return easeOutBack(t);
        case Type::InOutBack:
            return easeInOutBack(t);

        case Type::InElastic:
            return easeInElastic(t);
        case Type::OutElastic:
            return easeOutElastic(t);
        case Type::InOutElastic:
            return easeInOutElastic(t);

        case Type::InBounce:
            return easeInBounce(t);
        case Type::OutBounce:
            return easeOutBounce(t);
        case Type::InOutBounce:
            return easeInOutBounce(t);
    }

    return t;
}


std::string_view typeName(const Type type) noexcept {
    switch (type) {
        case Type::InSine:
            return "InSine";
        case Type::OutSine:
            return "OutSine";
        case Type::InOutSine:
            return "InOutSine";

        case Type::InQuad:
            return "InQuad";
        case Type::OutQuad:
            return "OutQuad";
        case Type::InOutQuad:
            return "InOutQuad";

        case Type::InCubic:
            return "InCubic";
        case Type::OutCubic:
            return "OutCubic";
        case Type::InOutCubic:
            return "InOutCubic";

        case Type::InQuart:
            return "InQuart";
        case Type::OutQuart:
            return "OutQuart";
        case Type::InOutQuart:
            return "InOutQuart";

        case Type::InQuint:
            return "InQuint";
        case Type::OutQuint:
            return "OutQuint";
        case Type::InOutQuint:
            return "InOutQuint";

        case Type::InExpo:
            return "InExpo";
        case Type::OutExpo:
            return "OutExpo";
        case Type::InOutExpo:
            return "InOutExpo";

        case Type::InCirc:
            return "InCirc";
        case Type::OutCirc:
            return "OutCirc";
        case Type::InOutCirc:
            return "InOutCirc";

        case Type::InBack:
            return "InBack";
        case Type::OutBack:
            return "OutBack";
        case Type::InOutBack:
            return "InOutBack";

        case Type::InElastic:
            return "InElastic";
        case Type::OutElastic:
            return "OutElastic";
        case Type::InOutElastic:
            return "InOutElastic";

        case Type::InBounce:
            return "InBounce";
        case Type::OutBounce:
            return "OutBounce";
        case Type::InOutBounce:
            return "InOutBounce";
    }

    return {};
}

}