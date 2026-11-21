
#include <grain/Geometry/PoissonDisc.hpp>
#include <grain/Geometry/PoissonDiscDensity.hpp>

#include <grain/Image/Image.hpp>
#include <grain/Image/ImageAccess.hpp>
#include <grain/Color/Color.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>
#include <numbers>
#include <iostream>


namespace Grain {


//------------------------------------------------------------------------------
//  PoissonDiscDensity
//------------------------------------------------------------------------------

auto PoissonDiscDensity::setSampler(
    PoissonDiscSampler* sampler
) -> void {

    sampler_ = sampler;

    if (sampler_) {
        min_radius_ = sampler_->minRadius();
        max_radius_ = sampler_->maxRadius();
        radius_delta_ = max_radius_ - min_radius_;
    }
}


auto PoissonDiscDensity::radiusAtPosition(
    const Vec2d& position
) -> double {

    (void)position;

    return min_radius_;
}


//------------------------------------------------------------------------------
//  PoissonDiscImageDensity
//------------------------------------------------------------------------------

PoissonDiscImageDensity::PoissonDiscImageDensity(
    Image* image
) {

    setImage(image);
}


PoissonDiscImageDensity::~PoissonDiscImageDensity() {

    setImage(nullptr);
}


//------------------------------------------------------------------------------
//  Sampler
//------------------------------------------------------------------------------

auto PoissonDiscImageDensity::setSampler(
    PoissonDiscSampler* sampler
) -> void {

    PoissonDiscDensity::setSampler(sampler);

    if (!sampler || !image_) {
        scale_.x = 1.0;
        scale_.y = 1.0;
        return;
    }

    const double sampler_width = sampler->width();
    const double sampler_height = sampler->height();

    if (sampler_width <= 0.0 || sampler_height <= 0.0) {
        scale_.x = 1.0;
        scale_.y = 1.0;
        return;
    }

    scale_.x = static_cast<double>(image_->width()) / sampler_width;
    scale_.y = static_cast<double>(image_->height()) / sampler_height;
}


//------------------------------------------------------------------------------
//  Image
//------------------------------------------------------------------------------

auto PoissonDiscImageDensity::setImage(
    Image* image
) -> void {

    if (image == image_) {
        return;
    }

    delete image_access_;
    image_access_ = nullptr;

    image_ = image;

    if (image_) {
        image_access_ = new ImageAccess(*image_);
    }

    scale_ = {1.0, 1.0};

    // Recalculate the scale if a sampler is already attached.
    if (sampler_) {
        setSampler(sampler_);
    }
}


//------------------------------------------------------------------------------
//  Radius
//------------------------------------------------------------------------------

auto PoissonDiscImageDensity::radiusAtPosition(
    const Vec2d& position
) -> double {

    if (!image_ || !image_access_ || !sampler_) {
        return min_radius_;
    }

    const Vec2d local_position = {
        position.x - sampler_->bounds().minX(),
        position.y - sampler_->bounds().minY()
    };

    if (!image_access_->readInterpolated(local_position * scale_)) {
        return min_radius_;
    }

    auto* pixel = image_access_->transferPtr();

    const Color color(
        pixel[0],
        pixel[1],
        pixel[2]
    );

    const double brightness = color.lumina709();

    return min_radius_ + brightness * brightness * radius_delta_;
}


//------------------------------------------------------------------------------
//  Construction
//------------------------------------------------------------------------------

PoissonDiscSampler::PoissonDiscSampler(
    const Boundsd& bounds,
    PoissonDiscDensity& density,
    double min_radius,
    double max_radius,
    int32_t max_tries,
    std::size_t max_points
)
    : bounds_(bounds),
      density_(density),
      min_radius_(min_radius),
      max_radius_(max_radius),
      max_tries_(std::max(max_tries, 2)),
      max_points_(max_points) {

    // Validate bounds.

    if (!bounds_.isValid() ||
        !std::isfinite(bounds_.minX()) ||
        !std::isfinite(bounds_.minY()) ||
        !std::isfinite(bounds_.maxX()) ||
        !std::isfinite(bounds_.maxY()) ||
        !std::isfinite(bounds_.width()) ||
        !std::isfinite(bounds_.height()) ||
        bounds_.width() <= 0.0 ||
        bounds_.height() <= 0.0) {

        throw std::invalid_argument(
            "PoissonDiscSampler: invalid sampling bounds"
        );
    }

    // Validate radii.

    if (!std::isfinite(min_radius_) ||
        !std::isfinite(max_radius_) ||
        min_radius_ <= 0.0 ||
        max_radius_ < min_radius_) {

        throw std::invalid_argument(
            "PoissonDiscSampler: invalid radius configuration"
        );
    }

    // Calculate grid dimensions.

    cell_size_ = min_radius_ / std::sqrt(2.0);

    if (!std::isfinite(cell_size_) || cell_size_ <= 0.0) {
        throw std::invalid_argument(
            "PoissonDiscSampler: invalid cell size"
        );
    }

    const double grid_width =
        std::ceil(bounds_.width() / cell_size_);

    const double grid_height =
        std::ceil(bounds_.height() / cell_size_);

    if (!std::isfinite(grid_width) ||
        !std::isfinite(grid_height) ||
        grid_width < 1.0 ||
        grid_height < 1.0 ||
        grid_width > std::numeric_limits<int32_t>::max() ||
        grid_height > std::numeric_limits<int32_t>::max()) {

        throw std::length_error(
            "PoissonDiscSampler: invalid grid dimensions"
        );
    }

    grid_width_ = static_cast<int32_t>(grid_width);
    grid_height_ = static_cast<int32_t>(grid_height);

    const std::size_t width =
        static_cast<std::size_t>(grid_width_);

    const std::size_t height =
        static_cast<std::size_t>(grid_height_);

    if (height > std::numeric_limits<std::size_t>::max() / width) {
        throw std::length_error(
            "PoissonDiscSampler: grid size overflow"
        );
    }

    const std::size_t grid_size = width * height;

    if (grid_size > grid_.max_size()) {
        throw std::length_error(
            "PoissonDiscSampler: grid is too large"
        );
    }

    grid_.resize(grid_size);

    // Initialize random generator.

    random_engine_.seed(std::random_device{}());

    // Register sampler with density.

    density_.setSampler(this);
}


PoissonDiscSampler::~PoissonDiscSampler() {

    density_.setSampler(nullptr);
}


//------------------------------------------------------------------------------
//  Sampling state
//------------------------------------------------------------------------------

auto PoissonDiscSampler::reset() -> void {

    active_points_.clear();

    std::fill(
        grid_.begin(),
        grid_.end(),
        GridCell{}
    );

    first_point_flag_ = true;
    grid_point_count_ = 0;
}


//------------------------------------------------------------------------------
//  Accessors
//------------------------------------------------------------------------------

auto PoissonDiscSampler::bounds() const noexcept -> const Boundsd& {

    return bounds_;
}


auto PoissonDiscSampler::width() const noexcept -> double {

    return bounds_.width();
}


auto PoissonDiscSampler::height() const noexcept -> double {

    return bounds_.height();
}


auto PoissonDiscSampler::minRadius() const noexcept -> double {

    return min_radius_;
}


auto PoissonDiscSampler::maxRadius() const noexcept -> double {

    return max_radius_;
}


auto PoissonDiscSampler::done() const noexcept -> bool {

    return !first_point_flag_ && active_points_.empty();
}


//------------------------------------------------------------------------------
//  Random numbers
//------------------------------------------------------------------------------

auto PoissonDiscSampler::random01() noexcept -> double {

    return std::generate_canonical<
        double,
        std::numeric_limits<double>::digits
    >(random_engine_);
}


auto PoissonDiscSampler::randomIndex(
    std::size_t count
) -> std::size_t {

    if (count == 0) {
        return 0;
    }

    std::uniform_int_distribution<std::size_t> distribution(
        0,
        count - 1
    );

    return distribution(random_engine_);
}


//------------------------------------------------------------------------------
//  Geometry helpers
//------------------------------------------------------------------------------

auto PoissonDiscSampler::distanceSquared(
    const Vec2d& a,
    const Vec2d& b
) const noexcept -> double {

    const double dx = a.x - b.x;
    const double dy = a.y - b.y;

    return dx * dx + dy * dy;
}


//------------------------------------------------------------------------------
//  Grid helpers
//------------------------------------------------------------------------------

auto PoissonDiscSampler::gridIndex(
    int32_t column,
    int32_t row
) const noexcept -> std::size_t {

    return static_cast<std::size_t>(column) +
           static_cast<std::size_t>(row) *
           static_cast<std::size_t>(grid_width_);
}


//------------------------------------------------------------------------------
//  Point validation
//------------------------------------------------------------------------------

auto PoissonDiscSampler::isValidPoint(
    const Vec2d& position
) -> bool {

    // Check sampling bounds.
    // The maximum edges are exclusive.

    if (!std::isfinite(position.x) ||
        !std::isfinite(position.y) ||
        position.x < bounds_.minX() ||
        position.x >= bounds_.maxX() ||
        position.y < bounds_.minY() ||
        position.y >= bounds_.maxY()) {

        return false;
    }

    // Determine candidate radius.

    const double candidate_radius = density_.radiusAtPosition(position);

    if (!std::isfinite(candidate_radius) ||
        candidate_radius < min_radius_ ||
        candidate_radius > max_radius_) {

        return false;
    }

    // Determine grid position.

    const int32_t column = static_cast<int32_t>(
        (position.x - bounds_.minX()) / cell_size_
    );

    const int32_t row = static_cast<int32_t>(
        (position.y - bounds_.minY()) / cell_size_
    );

    // Search far enough to include every possible neighbor.
    // The maximum radius is the largest possible exclusion distance.

    const int64_t search_range = static_cast<int64_t>(
        std::min(
            std::ceil(max_radius_ / cell_size_),
            static_cast<double>(std::max(grid_width_, grid_height_))
        )
    );

    for (int64_t dy = -search_range; dy <= search_range; ++dy) {

        for (int64_t dx = -search_range; dx <= search_range; ++dx) {

            const int64_t neighbor_column =
                static_cast<int64_t>(column) + dx;

            const int64_t neighbor_row =
                static_cast<int64_t>(row) + dy;

            if (neighbor_column < 0 ||
                neighbor_column >= grid_width_ ||
                neighbor_row < 0 ||
                neighbor_row >= grid_height_) {

                continue;
            }

            const GridCell& cell = grid_[
                gridIndex(
                    static_cast<int32_t>(neighbor_column),
                    static_cast<int32_t>(neighbor_row)
                )
            ];

            if (!cell.occupied) {
                continue;
            }

            const double neighbor_radius =
                density_.radiusAtPosition(cell.position);

            if (!std::isfinite(neighbor_radius) ||
                neighbor_radius < min_radius_ ||
                neighbor_radius > max_radius_) {

                return false;
            }

            const double required_distance = std::max(
                candidate_radius,
                neighbor_radius
            );

            if (distanceSquared(position, cell.position) <=
                required_distance * required_distance) {

                return false;
            }
        }
    }

    return true;
}


//------------------------------------------------------------------------------
//  Create point
//------------------------------------------------------------------------------

auto PoissonDiscSampler::createNewPoint(
    const Vec2d& position
) -> Vec2d {

    ++grid_point_count_;
    if ((grid_point_count_ % 1000) == 0) {
        std::cout << grid_point_count_ << std::endl;
    }

    const int32_t column = static_cast<int32_t>(
        (position.x - bounds_.minX()) / cell_size_
    );

    const int32_t row = static_cast<int32_t>(
        (position.y - bounds_.minY()) / cell_size_
    );

    GridCell& cell = grid_[gridIndex(column, row)];

    cell.position = position;
    cell.occupied = true;

    active_points_.push_back(position);

    return position;
}


//------------------------------------------------------------------------------
//  Next point
//------------------------------------------------------------------------------

auto PoissonDiscSampler::next() -> std::optional<Vec2d> {

    if (max_points_ > 0 && grid_point_count_ >= max_points_) {
        return std::nullopt;
    }

    // Generate the initial random point.

    if (first_point_flag_) {

        first_point_flag_ = false;

        const Vec2d position = {
            bounds_.minX() + bounds_.width() * random01(),
            bounds_.minY() + bounds_.height() * random01()
        };

        const double radius = density_.radiusAtPosition(position);

        if (!std::isfinite(radius) ||
            radius < min_radius_ ||
            radius > max_radius_) {

            return std::nullopt;
        }

        return createNewPoint(position);
    }

    // Continue sampling around active points.

    while (!active_points_.empty()) {

        const std::size_t index = randomIndex(active_points_.size());

        const Vec2d base = active_points_[index];

        double base_radius = density_.radiusAtPosition(base);

        if (!std::isfinite(base_radius) ||
            base_radius < min_radius_ ||
            base_radius > max_radius_) {

            active_points_.erase(active_points_.begin() + index);
            continue;
        }

        // Try generating candidates around this point.

        for (int32_t i = 0; i < max_tries_; ++i) {

            const double distance =
                base_radius * (1.0 + random01());

            const double angle =
                2.0 * std::numbers::pi * random01();

            const Vec2d candidate = {
                base.x + distance * std::cos(angle),
                base.y + distance * std::sin(angle)
            };

            if (isValidPoint(candidate)) {
                return createNewPoint(candidate);
            }
        }

        // No valid candidate found.
        // Remove this point from the active list.

        active_points_.erase(active_points_.begin() + index);
    }

    return std::nullopt;
}


//------------------------------------------------------------------------------
//  Generate all points
//------------------------------------------------------------------------------

auto PoissonDiscSampler::all() -> std::vector<Vec2d> {

    reset();

    std::vector<Vec2d> result;

    while (const auto position = next()) {
        result.push_back(*position);
    }

    return result;
}


} // namespace Grain
