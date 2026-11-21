
#pragma once

#include <grain/Geometry/Bounds.hpp>
#include <grain/Math/Vec2.hpp>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <random>
#include <vector>


namespace Grain {


class PoissonDiscDensity;


class PoissonDiscSampler {

private:

    struct GridCell {
        bool occupied = false;
        Vec2d position{};
    };


    //--------------------------------------------------------------------------
    //  Configuration
    //--------------------------------------------------------------------------

    Boundsd bounds_;
    PoissonDiscDensity& density_;

    double min_radius_ = 1.0;
    double max_radius_ = 1.0;
    double cell_size_ = 1.0;

    int32_t grid_width_ = 0;
    int32_t grid_height_ = 0;
    int32_t max_tries_ = 30;

    std::size_t max_points_ = 10000;
    std::size_t grid_point_count_ = 0;

    //--------------------------------------------------------------------------
    //  Sampling state
    //--------------------------------------------------------------------------

    std::vector<GridCell> grid_;
    std::vector<Vec2d> active_points_;

    std::mt19937_64 random_engine_;

    bool first_point_flag_ = true;


public:

    //--------------------------------------------------------------------------
    //  Construction
    //--------------------------------------------------------------------------

    PoissonDiscSampler(
        const Boundsd& bounds,
        PoissonDiscDensity& density,
        double min_radius,
        double max_radius,
        int32_t max_tries = 30,
        std::size_t max_points = 0
    );

    ~PoissonDiscSampler();

    PoissonDiscSampler(const PoissonDiscSampler&) = delete;
    auto operator = (const PoissonDiscSampler&) -> PoissonDiscSampler& = delete;

    PoissonDiscSampler(PoissonDiscSampler&&) = delete;
    auto operator = (PoissonDiscSampler&&) -> PoissonDiscSampler& = delete;


    //--------------------------------------------------------------------------
    //  Sampling
    //--------------------------------------------------------------------------

    auto reset() -> void;

    auto next() -> std::optional<Vec2d>;

    auto all() -> std::vector<Vec2d>;

    [[nodiscard]]
    auto done() const noexcept -> bool;


    //--------------------------------------------------------------------------
    //  Accessors
    //--------------------------------------------------------------------------

    [[nodiscard]]
    auto bounds() const noexcept -> const Boundsd&;

    [[nodiscard]]
    auto width() const noexcept -> double;

    [[nodiscard]]
    auto height() const noexcept -> double;

    [[nodiscard]]
    auto minRadius() const noexcept -> double;

    [[nodiscard]]
    auto maxRadius() const noexcept -> double;


private:

    //--------------------------------------------------------------------------
    //  Random numbers
    //--------------------------------------------------------------------------

    auto random01() noexcept -> double;

    auto randomIndex(std::size_t count) -> std::size_t;


    //--------------------------------------------------------------------------
    //  Sampling helpers
    //--------------------------------------------------------------------------

    [[nodiscard]]
    auto distanceSquared(
        const Vec2d& a,
        const Vec2d& b
    ) const noexcept -> double;

    auto isValidPoint(const Vec2d& position) -> bool;

    auto createNewPoint(const Vec2d& position) -> Vec2d;


    //--------------------------------------------------------------------------
    //  Grid helpers
    //--------------------------------------------------------------------------

    [[nodiscard]]
    auto gridIndex(
        int32_t column,
        int32_t row
    ) const noexcept -> std::size_t;

};


} // namespace Grain
