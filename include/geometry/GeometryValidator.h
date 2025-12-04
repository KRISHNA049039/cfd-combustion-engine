#pragma once

#include "geometry/GeometryReader.h"
#include <vector>
#include <string>
#include <map>
#include <set>

namespace cfd {

/**
 * @brief Geometry error information
 */
struct GeometryError {
    enum class Type {
        NON_MANIFOLD_EDGE,
        OPEN_EDGE,
        DEGENERATE_TRIANGLE,
        INCONSISTENT_NORMAL,
        DUPLICATE_TRIANGLE,
        SELF_INTERSECTION
    };
    
    Type type;
    std::string message;
    Vector3D location;
    
    GeometryError(Type t, const std::string& msg, const Vector3D& loc = Vector3D())
        : type(t), message(msg), location(loc) {}
    
    std::string getTypeString() const {
        switch (type) {
            case Type::NON_MANIFOLD_EDGE: return "Non-manifold edge";
            case Type::OPEN_EDGE: return "Open edge";
            case Type::DEGENERATE_TRIANGLE: return "Degenerate triangle";
            case Type::INCONSISTENT_NORMAL: return "Inconsistent normal";
            case Type::DUPLICATE_TRIANGLE: return "Duplicate triangle";
            case Type::SELF_INTERSECTION: return "Self-intersection";
            default: return "Unknown error";
        }
    }
};

/**
 * @brief Edge representation for manifold checking
 */
struct Edge {
    int v0, v1;  // Vertex indices (ordered: v0 < v1)
    
    Edge(int a, int b) {
        v0 = std::min(a, b);
        v1 = std::max(a, b);
    }
    
    bool operator<(const Edge& other) const {
        if (v0 != other.v0) return v0 < other.v0;
        return v1 < other.v1;
    }
    
    bool operator==(const Edge& other) const {
        return v0 == other.v0 && v1 == other.v1;
    }
};

/**
 * @brief Geometry validator for checking manifold surfaces and closed volumes
 */
class GeometryValidator {
public:
    GeometryValidator();
    
    // Main validation methods
    bool checkManifold(const std::vector<Surface>& surfaces);
    bool checkClosedVolume(const std::vector<Surface>& surfaces);
    bool validateAll(const std::vector<Surface>& surfaces);
    
    // Individual checks
    bool checkDegenerateTriangles(const std::vector<Surface>& surfaces);
    bool checkEdgeManifoldness(const std::vector<Surface>& surfaces);
    bool checkNormalConsistency(const std::vector<Surface>& surfaces);
    
    // Error reporting
    std::vector<GeometryError> getErrors() const { return errors; }
    int getNumErrors() const { return static_cast<int>(errors.size()); }
    void clearErrors() { errors.clear(); }
    
    // Statistics
    int getTotalTriangles() const { return totalTriangles; }
    int getTotalEdges() const { return totalEdges; }
    int getTotalVertices() const { return totalVertices; }
    
    // Tolerance settings
    void setDegeneracyTolerance(double tol) { degeneracyTolerance = tol; }
    void setNormalTolerance(double tol) { normalTolerance = tol; }
    
private:
    std::vector<GeometryError> errors;
    double degeneracyTolerance;
    double normalTolerance;
    
    int totalTriangles;
    int totalEdges;
    int totalVertices;
    
    // Helper methods
    void buildVertexMap(const std::vector<Surface>& surfaces,
                       std::map<Vector3D, int>& vertexMap,
                       std::vector<Vector3D>& vertices);
    
    void buildEdgeMap(const std::vector<Surface>& surfaces,
                     const std::map<Vector3D, int>& vertexMap,
                     std::map<Edge, int>& edgeCount);
    
    int getVertexIndex(const Vector3D& v, std::map<Vector3D, int>& vertexMap,
                      std::vector<Vector3D>& vertices);
    
    bool vectorsEqual(const Vector3D& v1, const Vector3D& v2, double tol = 1e-9) const;
};

// Helper for Vector3D comparison in map
inline bool operator<(const Vector3D& a, const Vector3D& b) {
    if (a.x != b.x) return a.x < b.x;
    if (a.y != b.y) return a.y < b.y;
    return a.z < b.z;
}

} // namespace cfd
