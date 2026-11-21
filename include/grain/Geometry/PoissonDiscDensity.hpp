
#pragma once

#include <grain/Math/Vec2.hpp>

namespace Grain {


class Image;
class ImageAccess;
class PoissonDiscSampler;


//------------------------------------------------------------------------------
//  PoissonDiscDensity
//------------------------------------------------------------------------------

class PoissonDiscDensity {

protected:

    PoissonDiscSampler* sampler_ = nullptr;

    double min_radius_ = 1.0;
    double max_radius_ = 1.0;
    double radius_delta_ = 0.0;


public:

    //--------------------------------------------------------------------------
    //  Construction
    //--------------------------------------------------------------------------

    PoissonDiscDensity() = default;

    virtual ~PoissonDiscDensity() = default;


    //--------------------------------------------------------------------------
    //  Sampler
    //--------------------------------------------------------------------------

    virtual auto setSampler(
        PoissonDiscSampler* sampler
    ) -> void;


    //--------------------------------------------------------------------------
    //  Density
    //--------------------------------------------------------------------------

    [[nodiscard]]
    virtual auto radiusAtPosition(
        const Vec2d& position
    ) -> double;

};


//------------------------------------------------------------------------------
//  PoissonDiscImageDensity
//------------------------------------------------------------------------------

class PoissonDiscImageDensity : public PoissonDiscDensity {

private:

    Image* image_ = nullptr;
    ImageAccess* image_access_ = nullptr;

    float pixel_[4]{};

    Vec2d scale_ = {1.0, 1.0};


public:

    //--------------------------------------------------------------------------
    //  Construction
    //--------------------------------------------------------------------------

    explicit PoissonDiscImageDensity(Image* image);

    ~PoissonDiscImageDensity() override;


    //--------------------------------------------------------------------------
    //  Sampler
    //--------------------------------------------------------------------------

    auto setSampler(
        PoissonDiscSampler* sampler
    ) -> void override;


    //--------------------------------------------------------------------------
    //  Image
    //--------------------------------------------------------------------------

    auto setImage(Image* image) -> void;


    //--------------------------------------------------------------------------
    //  Density
    //--------------------------------------------------------------------------

    [[nodiscard]]
    auto radiusAtPosition(
        const Vec2d& position
    ) -> double override;

};


} // namespace Grain
