#include "geometry/GeometryValidator.h"
#include <cmath>
#include <algorithm>

namespace cfd {

GeometryValidator::GeometryValidator()
    : degeneracyTolerance(1e-10), normalTolerance(0.1),
      totalTriangles(0), totalEdges(0), totalVertices(0) {
}

bool GeometryValidator::validateAll(const std::vector<Surface>& surfaces) {
    clearErrors();
    
    bool valid = true;
    valid &= checkDegenerateTriangles(surfaces);
    valid &= checkEdgeManifoldness(surfaces);
    valid &= checkClosedVolume(surfaces);
    valid &= checkNormalConsistency(surfaces);
    
    return valid;
}

bool GeometryValidator::checkDegenerateTriangles(const std::vector<Surface>& surfaces) {
    bool valid = true;
    totalTriangles = 0;
    
    for (const auto& surface : surfaces) {
        for (const auto& tri : surface.triangles) {
            totalTriangles++;
            
            double area = tri.area();
            if (area < degeneracyTolerance) {
                errors.push_back(GeometryError(
                    GeometryError::Type::DEGENERATE_TRIANGLE,
                    "Triangle has near-zero area: " + std::to_string(area),
                    tri.centroid()
                ));
                valid = false;
            }
            
            // Check for duplicate vertices
            for (int i = 0; i < 3; ++i) {
                for (int j = i + 1; j < 3; ++j) {
                    if (vectorsEqual(tri.vertices[i], tri.vertices[j], degeneracyTolerance)) {
                        errors.push_back(GeometryError(
                            GeometryError::Type::DEGENERATE_TRIANGLE,
                            "Triangle has duplicate vertices",
                            tri.centroid()
                        ));
                        valid = false;
                    }
                }
            }
        }
    }
    
    return valid;
}

bool GeometryValidator::checkManifold(const std::vector<Surface>& surfaces) {
    return checkEdgeManifoldness(surfaces);
}

bool GeometryValidator::checkEdgeManifoldness(const std::vector<Surface>& surfaces) {
    bool valid = true;
    
    // Build vertex map
    std::map<Vector3D, int> vertexMap;
    std::vector<Vector3D> vertices;
    buildVertexMap(surfaces, vertexMap, vertices);
    totalVertices = static_cast<int>(vertices.size());
    
    // Build edge map
    std::map<Edge, int> edgeCount;
    buildEdgeMap(surfaces, vertexMap, edgeCount);
    totalEdges = static_cast<int>(edgeCount.size());
    
    // Check edge counts
    for (const auto& pair : edgeCount) {
        const Edge& edge = pair.first;
        int count = pair.second;
        
        if (count > 2) {
            // Non-manifold edge (shared by more than 2 triangles)
            Vector3D v0 = vertices[edge.v0];
            Vector3D v1 = vertices[edge.v1];
            Vector3D midpoint = (v0 + v1) * 0.5;
            
            errors.push_back(GeometryError(
                GeometryError::Type::NON_MANIFOLD_EDGE,
                "Edge shared by " + std::to_string(count) + " triangles",
                midpoint
            ));
            valid = false;
        }
    }
    
    return valid;
}

bool GeometryValidator::checkClosedVolume(const std::vector<Surface>& surfaces) {
    bool valid = true;
    
    // Build vertex map
    std::map<Vector3D, int> vertexMap;
    std::vector<Vector3D> vertices;
    buildVertexMap(surfaces, vertexMap, vertices);
    
    // Build edge map
    std::map<Edge, int> edgeCount;
    buildEdgeMap(surfaces, vertexMap, edgeCount);
    
    // Check for open edges (edges with count != 2)
    int openEdgeCount = 0;
    for (const auto& pair : edgeCount) {
        const Edge& edge = pair.first;
        int count = pair.second;
        
        if (count == 1) {
            // Open edge (boundary edge)
            Vector3D v0 = vertices[edge.v0];
            Vector3D v1 = vertices[edge.v1];
            Vector3D midpoint = (v0 + v1) * 0.5;
            
            errors.push_back(GeometryError(
                GeometryError::Type::OPEN_EDGE,
                "Edge belongs to only one triangle (open boundary)",
                midpoint
            ));
            openEdgeCount++;
            valid = false;
        }
    }
    
    if (openEdgeCount > 0) {
        // Volume is not closed
        return false;
    }
    
    return valid;
}

bool GeometryValidator::checkNormalConsistency(const std::vector<Surface>& surfaces) {
    bool valid = true;
    
    // Build vertex map
    std::map<Vector3D, int> vertexMap;
    std::vector<Vector3D> vertices;
    buildVertexMap(surfaces, vertexMap, vertices);
    
    // For each edge, check if adjacent triangles have consistent normals
    std::map<Edge, std::vector<Vector3D>> edgeNormals;
    
    for (const auto& surface : surfaces) {
        for (const auto& tri : surface.triangles) {
            // Get vertex indices
            int v0 = getVertexIndex(tri.vertices[0], vertexMap, vertices);
            int v1 = getVertexIndex(tri.vertices[1], vertexMap, vertices);
            int v2 = getVertexIndex(tri.vertices[2], vertexMap, vertices);
            
            // Add normal for each edge
            edgeNormals[Edge(v0, v1)].push_back(tri.normal);
            edgeNormals[Edge(v1, v2)].push_back(tri.normal);
            edgeNormals[Edge(v2, v0)].push_back(tri.normal);
        }
    }
    
    // Check normal consistency
    for (const auto& pair : edgeNormals) {
        const auto& normals = pair.second;
        
        if (normals.size() == 2) {
            // Check if normals are pointing in similar directions
            double dot = normals[0].dot(normals[1]);
            
            if (dot < -normalTolerance) {
                // Normals pointing in opposite directions (inconsistent orientation)
                const Edge& edge = pair.first;
                Vector3D v0 = vertices[edge.v0];
                Vector3D v1 = vertices[edge.v1];
                Vector3D midpoint = (v0 + v1) * 0.5;
                
                errors.push_back(GeometryError(
                    GeometryError::Type::INCONSISTENT_NORMAL,
                    "Adjacent triangles have inconsistent normal orientation",
                    midpoint
                ));
                valid = false;
            }
        }
    }
    
    return valid;
}

void GeometryValidator::buildVertexMap(const std::vector<Surface>& surfaces,
                                      std::map<Vector3D, int>& vertexMap,
                                      std::vector<Vector3D>& vertices) {
    vertexMap.clear();
    vertices.clear();
    
    for (const auto& surface : surfaces) {
        for (const auto& tri : surface.triangles) {
            for (int i = 0; i < 3; ++i) {
                getVertexIndex(tri.vertices[i], vertexMap, vertices);
            }
        }
    }
}

void GeometryValidator::buildEdgeMap(const std::vector<Surface>& surfaces,
                                    const std::map<Vector3D, int>& vertexMap,
                                    std::map<Edge, int>& edgeCount) {
    edgeCount.clear();
    
    for (const auto& surface : surfaces) {
        for (const auto& tri : surface.triangles) {
            // Get vertex indices
            auto it0 = vertexMap.find(tri.vertices[0]);
            auto it1 = vertexMap.find(tri.vertices[1]);
            auto it2 = vertexMap.find(tri.vertices[2]);
            
            if (it0 != vertexMap.end() && it1 != vertexMap.end() && it2 != vertexMap.end()) {
                int v0 = it0->second;
                int v1 = it1->second;
                int v2 = it2->second;
                
                // Count each edge
                edgeCount[Edge(v0, v1)]++;
                edgeCount[Edge(v1, v2)]++;
                edgeCount[Edge(v2, v0)]++;
            }
        }
    }
}

int GeometryValidator::getVertexIndex(const Vector3D& v,
                                     std::map<Vector3D, int>& vertexMap,
                                     std::vector<Vector3D>& vertices) {
    auto it = vertexMap.find(v);
    if (it != vertexMap.end()) {
        return it->second;
    }
    
    int index = static_cast<int>(vertices.size());
    vertices.push_back(v);
    vertexMap[v] = index;
    return index;
}

bool GeometryValidator::vectorsEqual(const Vector3D& v1, const Vector3D& v2, double tol) const {
    return (v1 - v2).magnitude() < tol;
}

} // namespace cfd
