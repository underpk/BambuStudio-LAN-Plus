#ifndef slic3r_BitmapArrange_hpp_
#define slic3r_BitmapArrange_hpp_

#include "Arrange.hpp"
#include "ExPolygon.hpp"
#include "BoundingBox.hpp"
#include <vector>

namespace Slic3r { namespace arrangement {

// Bitmap-based collision detection for arrangement
// Unlike NFP algorithm, this properly handles hollow spaces in objects
class ArrangeBitmap {
public:
    ArrangeBitmap(const BoundingBox& bed_bbox, double precision_mm);

    // Rasterize an ExPolygon to the bitmap (respects holes)
    void rasterize(const ExPolygon& poly, const Point& offset);

    // Check if placing poly at offset would collide with existing occupied pixels
    bool collides(const ExPolygon& poly, const Point& offset) const;

    // Mark region as occupied after placement
    void mark_occupied(const ExPolygon& poly, const Point& offset);

    // Mark exterior of bed polygon as occupied (for non-rectangular beds)
    void mark_bed_exterior(const Polygon& bed);

    // Clear the bitmap
    void clear();

    // Get dimensions
    int width() const { return m_width_px; }
    int height() const { return m_height_px; }
    double precision() const { return m_precision; }

private:
    std::vector<uint8_t> m_data;
    int m_width_px;
    int m_height_px;
    double m_precision;  // mm per pixel
    BoundingBox m_bed_bbox;

    // Convert between scaled coordinates and pixel coordinates
    Point to_pixel(const Point& p) const;
    Point from_pixel(int x, int y) const;

    // Get/set pixel value
    bool get_pixel(int x, int y) const;
    void set_pixel(int x, int y, bool value);

    // Rasterize a single polygon (without holes) - internal use
    void rasterize_polygon(const Polygon& poly, const Point& offset, bool value);
};

// Bitmap-based arrangement algorithm
// This is an alternative to the NFP-based arrangement that properly handles hollow spaces
void arrange_bitmap(
    ArrangePolygons& items,
    const ArrangePolygons& excludes,
    const Points& bed,
    const ArrangeParams& params
);

}} // namespace Slic3r::arrangement

#endif // slic3r_BitmapArrange_hpp_
