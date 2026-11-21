#include "../../include/Grain.hpp"

#include <algorithm>
#include <cstdint>
#include <vector>

using namespace Grain;


static auto polygonCentroid(
    const std::vector<Vec2d>& vertices
) -> Vec2d {

    if (vertices.size() < 3) {
        return {};
    }

    double area = 0.0;
    double cx = 0.0;
    double cy = 0.0;

    for (std::size_t i = 0; i < vertices.size(); ++i) {

        const auto& a = vertices[i];
        const auto& b = vertices[(i + 1) % vertices.size()];

        const double cross = a.x * b.y - b.x * a.y;

        area += cross;
        cx += (a.x + b.x) * cross;
        cy += (a.y + b.y) * cross;
    }

    if (std::abs(area) < 1e-12) {
        return {};
    }

    const double factor = 1.0 / (3.0 * area);

    return {
        cx * factor,
        cy * factor
    };
}


//------------------------------------------------------------------------------
//  Delaunay / Voronoi image demo
//------------------------------------------------------------------------------

void delaunay(
    GraphicContext& gc,
    const Rectd& rect,
    int32_t seed_n,
    int32_t pass_n
) {

    if (seed_n < 3 || pass_n < 1 || !rect.isValid()) {
        return;
    }


    //--------------------------------------------------------------------------
    //  Initial seed points
    //--------------------------------------------------------------------------

    List<Vec2d> seed_pos;
    seed_pos.reserve(static_cast<std::size_t>(seed_n));

    Random random;

    for (int32_t i = 0; i < seed_n; ++i) {

        const double x = random.real(
            40.0,
            rect.width - 40.0
        );

        const double y = random.real(
            40.0,
            rect.height - 40.0
        );

        seed_pos.push_back({
            rect.x + x,
            rect.y + y
        });
    }


    //--------------------------------------------------------------------------
    //  Iterative Voronoi construction
    //--------------------------------------------------------------------------

    for (int32_t pass = 0; pass < pass_n; ++pass) {

        Delaunay32d delaunay(seed_pos);

        const Rectd bounds{
            rect.x + 100.0,
            rect.y + 100.0,
            rect.width - 200.0,
            rect.height - 200.0
        };

        delaunay.setBounds(bounds);
        delaunay.update();


        //---------------------------------------------------------------------- 
        //  Draw background
        //----------------------------------------------------------------------

        gc.setFillColor({0, 0, 0, 1});
        gc.fillRect(rect);


        //---------------------------------------------------------------------- 
        //  Draw Delaunay triangulation
        //----------------------------------------------------------------------

        gc.setFillColor({1.0f, 0.0f, 0.3f, 1});
        gc.setStrokeColor({0.3f, 0.3f, 1.0f});
        gc.setStrokeWidth(1.0);

        const auto& tri_indices = delaunay.triangleIndexList();

        for (std::size_t i = 0; i + 2 < tri_indices.size(); i += 3) {

            gc.beginPath();

            gc.moveTo(delaunay.triangleVertexAtIndex(
                static_cast<int32_t>(i)
            ));

            gc.lineTo(delaunay.triangleVertexAtIndex(
                static_cast<int32_t>(i + 1)
            ));

            gc.lineTo(delaunay.triangleVertexAtIndex(
                static_cast<int32_t>(i + 2)
            ));

            gc.closePath();
            gc.strokePath();
        }


        //---------------------------------------------------------------------- 
        //  Draw seed points
        //----------------------------------------------------------------------

        gc.setFillColor({1.0f, 0.0f, 0.3f, 1});

        for (const auto& vertex : delaunay.triangleVertexList()) {
            gc.fillCircle(vertex, 2.0);
        }


        //---------------------------------------------------------------------- 
        //  Calculate Voronoi cells
        //----------------------------------------------------------------------

        delaunay.computeVoronoiCells();

        std::fprintf(
            stderr,
            "Seeds: %zu, Triangle indices: %zu, Voronoi cells: %u\n",
            seed_pos.size(),
            tri_indices.size(),
            static_cast<unsigned>(delaunay.voronoiCellCount())
        );


        const auto& cells = delaunay.voronoiCells();
        const auto cell_sizes = delaunay.voronoiCellSizes();

        seed_pos.clear();
        seed_pos.reserve(cells.size());


        //---------------------------------------------------------------------- 
        //  Draw Voronoi cells and calculate new seed positions
        //----------------------------------------------------------------------

        for (std::size_t i = 0; i < cells.size(); ++i) {

            const auto& cell = cells[i];

            if (cell.size() < 3) {
                continue;
            }


            // Draw cell

            gc.beginPath();
            gc.moveTo(cell.front());

            for (std::size_t j = 1; j < cell.size(); ++j) {
                gc.lineTo(cell[j]);
            }

            gc.closePath();

            gc.setFillColor(Color(random.real(), random.real(), random.real(), 1));

            gc.fillPath();

            gc.setStrokeColor({0.3f, 1.0f, 0.5f, 1});
            gc.setStrokeWidth(1.0);
            gc.strokePath();


            // Calculate the new seed position

            const Vec2d centroid = polygonCentroid(cell);

            if (rect.contains(centroid)) {
                seed_pos.push_back(centroid);
            }

            gc.setFillColor({1.0f, 1.0f, 1.0f, 1});
            gc.fillCircle(centroid, 2.0);
        }
    }
}


auto main() -> int {
    constexpr uint32_t width = 4000;
    constexpr uint32_t height = 4000;

    Grain::Image image;

    if (!image.create(
        width,
        height,
        4,
        Grain::Image::PixelType::Float32
    )) {
        std::fprintf(stderr, "Failed to create image.\n");
        return 1;
    }


    auto gc = Grain::GraphicContext::createGraphicContext(image);
    if (gc == nullptr) {
        return 1;
    }


    delaunay(
        *gc,
        Rectd(0.0, 0.0, image.width(), image.height()),
        2000, // seed_n,
        4 // pass_n
    );


    const auto error = ImageIO::save(
        image,
        "delaunay_image.png",
        ImageIO::Format::PNG,
        100,
        true);

    if (error == ImageIO::Error::None) {
        return 1;
    }


    return 0;
}