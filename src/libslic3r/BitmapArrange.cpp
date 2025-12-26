#include "BitmapArrange.hpp"
#include "ClipperUtils.hpp"
#include "Geometry.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <fstream>

#include <boost/log/trivial.hpp>

// Debug log file
static std::ofstream g_debug_log;
static void init_debug_log() {
    if (!g_debug_log.is_open()) {
        g_debug_log.open("E:/Desktop/bitmap_arrange_debug.txt", std::ios::out | std::ios::trunc);
    }
}
#define DEBUG_LOG(x) do { init_debug_log(); g_debug_log << x << std::endl; g_debug_log.flush(); } while(0)

namespace Slic3r { namespace arrangement {

// Simple bitmap class for fast collision detection (like Plater)
class FastBitmap {
public:
    int width, height;
    std::vector<uint8_t> data;
    int centerX, centerY;  // Gravity center in pixels

    FastBitmap(int w, int h) : width(w), height(h), data(w * h, 0), centerX(w/2), centerY(h/2) {}

    inline bool getPoint(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height) return false;
        return data[y * width + x] != 0;
    }

    inline void setPoint(int x, int y, uint8_t val = 1) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            data[y * width + x] = val;
        }
    }

    // Check if this bitmap overlaps with another at offset (offx, offy)
    bool overlaps(const FastBitmap& other, int offx, int offy) const {
        // Iterate over this bitmap's pixels
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (getPoint(x, y) && other.getPoint(x + offx, y + offy)) {
                    return true;
                }
            }
        }
        return false;
    }

    // Write another bitmap onto this one at offset
    void write(const FastBitmap& other, int offx, int offy) {
        for (int y = 0; y < other.height; y++) {
            for (int x = 0; x < other.width; x++) {
                if (other.getPoint(x, y)) {
                    setPoint(x + offx, y + offy, other.getPoint(x, y));
                }
            }
        }
    }

    // Dilate the bitmap by N iterations (for spacing)
    void dilate(int iterations) {
        for (int i = 0; i < iterations; i++) {
            std::vector<uint8_t> old_data = data;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    if (old_data[y * width + x] == 0) {
                        // Check 8-neighbors
                        bool has_neighbor = false;
                        for (int dy = -1; dy <= 1 && !has_neighbor; dy++) {
                            for (int dx = -1; dx <= 1 && !has_neighbor; dx++) {
                                int nx = x + dx, ny = y + dy;
                                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                                    if (old_data[ny * width + nx] != 0) {
                                        has_neighbor = true;
                                    }
                                }
                            }
                        }
                        if (has_neighbor) {
                            data[y * width + x] = 1;
                        }
                    }
                }
            }
        }
    }

    // Dilate only outer edges (not holes) - uses flood fill to identify exterior
    void dilate_outer_only(int iterations) {
        if (iterations <= 0) return;

        // First, flood fill from edges to mark exterior region
        std::vector<uint8_t> exterior(width * height, 0);
        std::vector<std::pair<int,int>> queue;

        // Add all edge pixels that are empty to queue
        for (int x = 0; x < width; x++) {
            if (data[0 * width + x] == 0 && exterior[0 * width + x] == 0) {
                exterior[0 * width + x] = 1;
                queue.push_back({x, 0});
            }
            if (data[(height-1) * width + x] == 0 && exterior[(height-1) * width + x] == 0) {
                exterior[(height-1) * width + x] = 1;
                queue.push_back({x, height-1});
            }
        }
        for (int y = 0; y < height; y++) {
            if (data[y * width + 0] == 0 && exterior[y * width + 0] == 0) {
                exterior[y * width + 0] = 1;
                queue.push_back({0, y});
            }
            if (data[y * width + width-1] == 0 && exterior[y * width + width-1] == 0) {
                exterior[y * width + width-1] = 1;
                queue.push_back({width-1, y});
            }
        }

        // Flood fill using 4-connectivity (to be conservative about holes)
        size_t head = 0;
        while (head < queue.size()) {
            int x = queue[head].first;
            int y = queue[head].second;
            head++;

            const int dx[] = {0, 0, 1, -1};
            const int dy[] = {1, -1, 0, 0};
            for (int d = 0; d < 4; d++) {
                int nx = x + dx[d];
                int ny = y + dy[d];
                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    if (data[ny * width + nx] == 0 && exterior[ny * width + nx] == 0) {
                        exterior[ny * width + nx] = 1;
                        queue.push_back({nx, ny});
                    }
                }
            }
        }

        // Now dilate: only fill exterior pixels adjacent to occupied pixels
        for (int iter = 0; iter < iterations; iter++) {
            std::vector<uint8_t> old_data = data;
            std::vector<uint8_t> old_exterior = exterior;

            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    // Only consider exterior empty pixels
                    if (old_exterior[y * width + x] && old_data[y * width + x] == 0) {
                        // Check if adjacent to occupied pixel
                        bool adjacent_to_occupied = false;
                        for (int dy = -1; dy <= 1 && !adjacent_to_occupied; dy++) {
                            for (int dx = -1; dx <= 1 && !adjacent_to_occupied; dx++) {
                                int nx = x + dx, ny = y + dy;
                                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                                    if (old_data[ny * width + nx] != 0) {
                                        adjacent_to_occupied = true;
                                    }
                                }
                            }
                        }
                        if (adjacent_to_occupied) {
                            data[y * width + x] = 1;
                            exterior[y * width + x] = 0; // No longer exterior
                        }
                    }
                }
            }
        }
    }
};

// Rasterize a polygon to a FastBitmap
static FastBitmap* rasterize_polygon(const ExPolygon& poly, double precision, int dilate_pixels = 0) {
    BoundingBox bbox = get_extents(poly);

    // Add extra margin for dilatation so shape can expand
    int margin = dilate_pixels + 1;
    int width = static_cast<int>(std::ceil(unscaled(bbox.size().x()) / precision)) + 2 + 2 * dilate_pixels;
    int height = static_cast<int>(std::ceil(unscaled(bbox.size().y()) / precision)) + 2 + 2 * dilate_pixels;

    FastBitmap* bmp = new FastBitmap(width, height);

    // Compute center offset (account for dilate margin)
    Point center = bbox.center();
    bmp->centerX = static_cast<int>((unscaled(center.x()) - unscaled(bbox.min.x())) / precision) + 1 + dilate_pixels;
    bmp->centerY = static_cast<int>((unscaled(center.y()) - unscaled(bbox.min.y())) / precision) + 1 + dilate_pixels;

    // Offset for rasterization (account for margin)
    int raster_offset = 1 + dilate_pixels;

    // Rasterize using scanline for initial bitmap creation only
    auto to_pixel = [&](coord_t v, coord_t min_v) -> int {
        return static_cast<int>((unscaled(v) - unscaled(min_v)) / precision) + raster_offset;
    };

    // Rasterize contour
    const Points& pts = poly.contour.points;
    size_t n = pts.size();
    if (n < 3) return bmp;

    // Scanline rasterization with offset
    int base_height = height - 2 * raster_offset;  // Original polygon height in pixels
    for (int py = 0; py < base_height; py++) {
        double scan_y = unscaled(bbox.min.y()) + py * precision;
        coord_t scan_y_scaled = scaled(scan_y);

        std::vector<double> intersections;

        for (size_t i = 0; i < n; i++) {
            const Point& p1 = pts[i];
            const Point& p2 = pts[(i + 1) % n];

            if ((p1.y() <= scan_y_scaled && p2.y() > scan_y_scaled) ||
                (p2.y() <= scan_y_scaled && p1.y() > scan_y_scaled)) {
                double t = static_cast<double>(scan_y_scaled - p1.y()) / (p2.y() - p1.y());
                double x = unscaled(p1.x()) + t * unscaled(p2.x() - p1.x());
                intersections.push_back(x);
            }
        }

        std::sort(intersections.begin(), intersections.end());

        for (size_t i = 0; i + 1 < intersections.size(); i += 2) {
            int x_start = static_cast<int>((intersections[i] - unscaled(bbox.min.x())) / precision) + raster_offset;
            int x_end = static_cast<int>((intersections[i + 1] - unscaled(bbox.min.x())) / precision) + raster_offset;
            x_start = std::max(0, x_start);
            x_end = std::min(width - 1, x_end);
            for (int px = x_start; px <= x_end; px++) {
                bmp->setPoint(px, py + raster_offset, 1);
            }
        }
    }

    // Rasterize holes (unfill)
    for (const Polygon& hole : poly.holes) {
        const Points& hpts = hole.points;
        size_t hn = hpts.size();
        if (hn < 3) continue;

        for (int py = 0; py < base_height; py++) {
            double scan_y = unscaled(bbox.min.y()) + py * precision;
            coord_t scan_y_scaled = scaled(scan_y);

            std::vector<double> intersections;

            for (size_t i = 0; i < hn; i++) {
                const Point& p1 = hpts[i];
                const Point& p2 = hpts[(i + 1) % hn];

                if ((p1.y() <= scan_y_scaled && p2.y() > scan_y_scaled) ||
                    (p2.y() <= scan_y_scaled && p1.y() > scan_y_scaled)) {
                    double t = static_cast<double>(scan_y_scaled - p1.y()) / (p2.y() - p1.y());
                    double x = unscaled(p1.x()) + t * unscaled(p2.x() - p1.x());
                    intersections.push_back(x);
                }
            }

            std::sort(intersections.begin(), intersections.end());

            for (size_t i = 0; i + 1 < intersections.size(); i += 2) {
                int x_start = static_cast<int>((intersections[i] - unscaled(bbox.min.x())) / precision) + raster_offset;
                int x_end = static_cast<int>((intersections[i + 1] - unscaled(bbox.min.x())) / precision) + raster_offset;
                x_start = std::max(0, x_start);
                x_end = std::min(width - 1, x_end);
                for (int px = x_start; px <= x_end; px++) {
                    bmp->setPoint(px, py + raster_offset, 0);
                }
            }
        }
    }

    // Dilate for spacing
    if (dilate_pixels > 0) {
        bmp->dilate(dilate_pixels);
    }

    return bmp;
}

// Rotate a bitmap (like Plater)
static FastBitmap* rotate_bitmap(const FastBitmap* src, double angle) {
    if (std::abs(angle) < 0.001) {
        // No rotation, just copy
        FastBitmap* dst = new FastBitmap(src->width, src->height);
        dst->data = src->data;
        dst->centerX = src->centerX;
        dst->centerY = src->centerY;
        return dst;
    }

    double r = -angle;
    double cos_r = std::cos(r);
    double sin_r = std::sin(r);

    // Compute rotated bounding box
    double w = src->width;
    double h = src->height;

    double ax = w * cos_r - h * sin_r;
    double ay = w * sin_r + h * cos_r;
    double bx = -h * sin_r;
    double by = h * cos_r;
    double cx = w * cos_r;
    double cy = w * sin_r;

    double xMin = std::min({0.0, ax, bx, cx});
    double xMax = std::max({0.0, ax, bx, cx});
    double yMin = std::min({0.0, ay, by, cy});
    double yMax = std::max({0.0, ay, by, cy});

    int new_width = static_cast<int>(std::ceil(xMax - xMin));
    int new_height = static_cast<int>(std::ceil(yMax - yMin));

    FastBitmap* dst = new FastBitmap(new_width, new_height);

    double old_cx = src->centerX;
    double old_cy = src->centerY;
    double new_cx = new_width / 2.0;
    double new_cy = new_height / 2.0;

    // Compute new center position
    dst->centerX = static_cast<int>(new_cx);
    dst->centerY = static_cast<int>(new_cy);

    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < new_width; x++) {
            double dx = x - new_cx;
            double dy = y - new_cy;
            int srcX = static_cast<int>(std::round(cos_r * dx - sin_r * dy + old_cx));
            int srcY = static_cast<int>(std::round(sin_r * dx + cos_r * dy + old_cy));
            if (src->getPoint(srcX, srcY)) {
                dst->setPoint(x, y, 1);
            }
        }
    }

    return dst;
}

// Result of a single placement attempt
struct PlacementResult {
    Vec2crd translation;
    double rotation;
    int bed_idx;
    bool placed;
};

// Try one arrangement configuration and return results + score
static double try_arrangement(
    const ArrangePolygons& items,
    const std::vector<size_t>& order,
    const std::vector<double>& rotations,
    const BoundingBox& bed_bbox,
    double precision,
    int spacing_pixels,
    int step_pixels,
    int plate_width,
    int plate_height,
    std::vector<PlacementResult>& results)
{
    FastBitmap plate(plate_width, plate_height);
    double plate_cx = plate_width / 2.0;
    double plate_cy = plate_height / 2.0;

    results.resize(items.size());
    for (auto& r : results) {
        r.placed = false;
        r.bed_idx = UNARRANGED;
    }

    // Track bounding box of placed items for scoring
    double min_x = std::numeric_limits<double>::max();
    double min_y = std::numeric_limits<double>::max();
    double max_x = std::numeric_limits<double>::lowest();
    double max_y = std::numeric_limits<double>::lowest();
    int placed_count = 0;

    for (size_t idx : order) {
        const ArrangePolygon& item = items[idx];

        // Pre-rasterize part at all rotations
        std::vector<FastBitmap*> part_bmps;
        std::vector<FastBitmap*> part_bmps_dilated;

        for (double rot : rotations) {
            ExPolygon rotated_poly = item.poly;
            if (rot != 0) {
                rotated_poly.rotate(rot);
            }
            FastBitmap* bmp = rasterize_polygon(rotated_poly, precision, 0);
            part_bmps.push_back(bmp);
            FastBitmap* bmp_dilated = rasterize_polygon(rotated_poly, precision, spacing_pixels);
            part_bmps_dilated.push_back(bmp_dilated);
        }

        // Find best position using spiral search from center
        bool found = false;
        int best_x = 0, best_y = 0;
        size_t best_rot_idx = 0;
        int max_radius = std::max(plate_width, plate_height);

        for (int radius = 0; radius <= max_radius && !found; radius += step_pixels) {
            for (size_t rot_idx = 0; rot_idx < rotations.size() && !found; rot_idx++) {
                FastBitmap* part_bmp_dilated = part_bmps_dilated[rot_idx];
                int center_x = static_cast<int>(plate_cx) - part_bmp_dilated->centerX;
                int center_y = static_cast<int>(plate_cy) - part_bmp_dilated->centerY;

                for (int dy = -radius; dy <= radius && !found; dy += step_pixels) {
                    for (int dx = -radius; dx <= radius && !found; dx += step_pixels) {
                        if (radius > 0 && std::abs(dx) < radius && std::abs(dy) < radius)
                            continue;

                        int x = center_x + dx;
                        int y = center_y + dy;

                        if (x < 0 || y < 0 ||
                            x + part_bmp_dilated->width > plate_width ||
                            y + part_bmp_dilated->height > plate_height)
                            continue;

                        if (!part_bmp_dilated->overlaps(plate, x, y)) {
                            found = true;
                            best_x = x;
                            best_y = y;
                            best_rot_idx = rot_idx;
                        }
                    }
                }
            }
        }

        if (found) {
            FastBitmap* best_bmp = part_bmps[best_rot_idx];
            FastBitmap* best_bmp_dilated = part_bmps_dilated[best_rot_idx];

            int offset_x = best_x + (best_bmp_dilated->width - best_bmp->width) / 2;
            int offset_y = best_y + (best_bmp_dilated->height - best_bmp->height) / 2;
            plate.write(*best_bmp, offset_x, offset_y);

            // Compute center position in mm
            double placed_cx_mm = (offset_x + best_bmp->centerX) * precision;
            double placed_cy_mm = (offset_y + best_bmp->centerY) * precision;

            coord_t bed_x = bed_bbox.min.x() + scaled(placed_cx_mm);
            coord_t bed_y = bed_bbox.min.y() + scaled(placed_cy_mm);

            results[idx].translation = Vec2crd(bed_x, bed_y);
            results[idx].rotation = rotations[best_rot_idx];
            results[idx].bed_idx = 0;
            results[idx].placed = true;
            placed_count++;

            // Update bounding box (using bitmap extents)
            double bmp_min_x = offset_x * precision;
            double bmp_min_y = offset_y * precision;
            double bmp_max_x = (offset_x + best_bmp->width) * precision;
            double bmp_max_y = (offset_y + best_bmp->height) * precision;
            min_x = std::min(min_x, bmp_min_x);
            min_y = std::min(min_y, bmp_min_y);
            max_x = std::max(max_x, bmp_max_x);
            max_y = std::max(max_y, bmp_max_y);
        }

        // Cleanup
        for (auto bmp : part_bmps) delete bmp;
        for (auto bmp : part_bmps_dilated) delete bmp;
    }

    // Score: smaller bounding box area is better (lower score = better)
    // Also consider how compact the arrangement is (less empty space in bbox)
    double bbox_area = (max_x - min_x) * (max_y - min_y);

    // Count actual occupied pixels in the plate bitmap for density calculation
    int occupied_pixels = 0;
    for (int i = 0; i < plate_width * plate_height; i++) {
        if (plate.data[i]) occupied_pixels++;
    }
    double occupied_area = occupied_pixels * precision * precision;

    // Density ratio: higher is better (more of bbox is actually used)
    double density = (bbox_area > 0) ? (occupied_area / bbox_area) : 0;

    // Score combines bbox area (smaller better) and density (higher better)
    // Lower score = better arrangement
    double score = bbox_area * (2.0 - density);  // Penalize low density

    // Huge penalty for unplaced items
    score += (items.size() - placed_count) * 1000000;

    return score;
}

// Main bitmap arrangement function (Plater-style with multiple attempts)
void arrange_bitmap(
    ArrangePolygons& items,
    const ArrangePolygons& excludes,
    const Points& bed,
    const ArrangeParams& params)
{
    DEBUG_LOG("========== arrange_bitmap START (Plater-style) ==========");
    DEBUG_LOG("Number of items: " << items.size());

    if (items.empty()) return;

    BoundingBox bed_bbox = BoundingBox(bed);
    double bed_width_mm = unscaled(bed_bbox.size().x());
    double bed_height_mm = unscaled(bed_bbox.size().y());

    DEBUG_LOG("Bed size: " << bed_width_mm << " x " << bed_height_mm << " mm");

    // Settings
    double precision = 0.5;
    double spacing_mm = 1.0;
    double delta = 0.5;

    int spacing_pixels = static_cast<int>(std::ceil(spacing_mm / precision));
    int step_pixels = std::max(1, static_cast<int>(std::ceil(delta / precision)));

    int plate_width = static_cast<int>(std::ceil(bed_width_mm / precision));
    int plate_height = static_cast<int>(std::ceil(bed_height_mm / precision));

    DEBUG_LOG("Plate bitmap size: " << plate_width << " x " << plate_height << " pixels");

    // Rotation angles
    std::vector<double> rotations = {0.0};
    if (params.allow_rotations) {
        rotations = {0.0, M_PI / 2, M_PI, 3 * M_PI / 2};
    }

    // Sort by area (largest first) - this gives best packing results
    std::vector<size_t> order(items.size());
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(), [&items](size_t a, size_t b) {
        if (items[a].priority != items[b].priority)
            return items[a].priority > items[b].priority;
        return std::abs(items[a].poly.contour.area()) > std::abs(items[b].poly.contour.area());
    });

    // Run single arrangement pass
    std::vector<PlacementResult> best_results;
    double best_score = try_arrangement(items, order, rotations,
                                        bed_bbox, precision, spacing_pixels, step_pixels,
                                        plate_width, plate_height, best_results);

    DEBUG_LOG("Arrangement score: " << best_score);

    // Apply best results
    int placed_count = 0;
    for (size_t i = 0; i < items.size(); i++) {
        if (best_results[i].placed) {
            items[i].translation = best_results[i].translation;
            items[i].rotation = best_results[i].rotation;
            items[i].bed_idx = best_results[i].bed_idx;
            placed_count++;

            if (params.on_packed) {
                params.on_packed(items[i]);
            }
        } else {
            items[i].bed_idx = UNARRANGED;
        }
    }

    if (params.progressind) {
        params.progressind(0, "");
    }

    DEBUG_LOG("========== arrange_bitmap END ==========");
    DEBUG_LOG("Best score: " << best_score << ", placed " << placed_count << " of " << items.size() << " items");
}

// Keep the old ArrangeBitmap class for compatibility but it's not used now
ArrangeBitmap::ArrangeBitmap(const BoundingBox& bed_bbox, double precision_mm)
    : m_precision(precision_mm)
    , m_bed_bbox(bed_bbox)
{
    double width_mm = unscaled(bed_bbox.size().x());
    double height_mm = unscaled(bed_bbox.size().y());
    m_width_px = static_cast<int>(std::ceil(width_mm / precision_mm)) + 1;
    m_height_px = static_cast<int>(std::ceil(height_mm / precision_mm)) + 1;
    m_data.resize(m_width_px * m_height_px, 0);
}

void ArrangeBitmap::clear() { std::fill(m_data.begin(), m_data.end(), 0); }
Point ArrangeBitmap::to_pixel(const Point& p) const { return Point(0, 0); }
Point ArrangeBitmap::from_pixel(int x, int y) const { return Point(0, 0); }
bool ArrangeBitmap::get_pixel(int x, int y) const { return false; }
void ArrangeBitmap::set_pixel(int x, int y, bool value) {}
void ArrangeBitmap::rasterize_polygon(const Polygon& poly, const Point& offset, bool value) {}
void ArrangeBitmap::rasterize(const ExPolygon& poly, const Point& offset) {}
void ArrangeBitmap::mark_occupied(const ExPolygon& poly, const Point& offset) {}
bool ArrangeBitmap::collides(const ExPolygon& poly, const Point& offset) const { return true; }
void ArrangeBitmap::mark_bed_exterior(const Polygon& bed) {}

}} // namespace Slic3r::arrangement
