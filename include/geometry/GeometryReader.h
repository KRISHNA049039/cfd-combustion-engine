#pragma once

#include "core/Vector3D.h"
#include <vector>
#include <string>
#include <memory>

namespace cfd {

/**
 * @brief Surface triangle in 3D space
 */
struct Triangle {
    Vector3D vertices[3];
    Vector3D normal;
    
    Triangle() {}
    Triangle(const Vector3D& v0, const Vector3D& v1, const Vector3D& v2)
        : vertices{v0, v1, v2} {
        computeNormal();
    }
    
    void computeNormal() {
        Vector3D v1 = vertices[1] - vertices[0];
        Vector3D v2 = vertices[2] - vertices[0];
        normal = v1.cross(v2).normalized();
    }
    
    Vector3D centroid() const {
        return (vertices[0] + vertices[1] + vertices[2]) / 3.0;
    }
    
    double area() const {
        Vector3D v1 = vertices[1] - vertices[0];
        Vector3D v2 = vertices[2] - vertices[0];
        return 0.5 * v1.cross(v2).magnitude();
    }
};

/**
 * @brief Surface representation
 */
struct Surface {
    std::vector<Triangle> triangles;
    std::string name;
    
    Surface() {}
    Surface(const std::string& name_) : name(name_) {}
    
    int getNumTriangles() const { return static_cast<int>(triangles.size()); }
    void addTriangle(const Triangle& tri) { triangles.push_back(tri); }
};

/**
 * @brief Bounding box
 */
struct BoundingBox {
    Vector3D min;
    Vector3D max;
    
    BoundingBox() 
        : min(1e10, 1e10, 1e10), max(-1e10, -1e10, -1e10) {}
    
    void expand(const Vector3D& point) {
        min.x = std::min(min.x, point.x);
        min.y = std::min(min.y, point.y);
        min.z = std::min(min.z, point.z);
        max.x = std::max(max.x, point.x);
        max.y = std::max(max.y, point.y);
        max.z = std::max(max.z, point.z);
    }
    
    Vector3D center() const {
        return (min + max) * 0.5;
    }
    
    Vector3D size() const {
        return max - min;
    }
    
    double volume() const {
        Vector3D s = size();
        return s.x * s.y * s.z;
    }
};

/**
 * @brief Abstract base class for geometry readers
 */
class GeometryReader {
public:
    virtual ~GeometryReader() = default;
    
    // Pure virtual methods
    virtual bool loadFile(const std::string& filename) = 0;
    virtual std::vector<Surface> getSurfaces() const = 0;
    virtual BoundingBox getBounds() const = 0;
    virtual bool validate() const = 0;
    
    // Common functionality
    int getNumTriangles() const;
    void scale(double factor);
    void translate(const Vector3D& offset);
    
protected:
    std::vector<Surface> surfaces;
    BoundingBox bounds;
    bool loaded;
    
    GeometryReader() : loaded(false) {}
    
    void computeBounds();
};

} // namespace cfd
