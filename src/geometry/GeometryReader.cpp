#include "geometry/GeometryReader.h"

namespace cfd {

int GeometryReader::getNumTriangles() const {
    int count = 0;
    for (const auto& surface : surfaces) {
        count += surface.getNumTriangles();
    }
    return count;
}

void GeometryReader::computeBounds() {
    bounds = BoundingBox();
    
    for (const auto& surface : surfaces) {
        for (const auto& tri : surface.triangles) {
            for (int i = 0; i < 3; ++i) {
                bounds.expand(tri.vertices[i]);
            }
        }
    }
}

void GeometryReader::scale(double factor) {
    for (auto& surface : surfaces) {
        for (auto& tri : surface.triangles) {
            for (int i = 0; i < 3; ++i) {
                tri.vertices[i] *= factor;
            }
            tri.computeNormal();
        }
    }
    computeBounds();
}

void GeometryReader::translate(const Vector3D& offset) {
    for (auto& surface : surfaces) {
        for (auto& tri : surface.triangles) {
            for (int i = 0; i < 3; ++i) {
                tri.vertices[i] += offset;
            }
        }
    }
    computeBounds();
}

} // namespace cfd
