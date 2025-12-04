#pragma once

#include "geometry/GeometryReader.h"
#include "core/Vector3D.h"
#include <vector>
#include <string>
#include <map>

namespace cfd {

/**
 * @brief Boundary region information
 */
struct BoundaryRegion {
    std::string name;
    std::vector<int> triangleIndices;
    Vector3D averageNormal;
    Vector3D centroid;
    double totalArea;
    
    BoundaryRegion() : totalArea(0.0) {}
    BoundaryRegion(const std::string& name_) : name(name_), totalArea(0.0) {}
};

/**
 * @brief Extracts and identifies boundary surfaces from geometry
 */
class BoundaryExtractor {
public:
    BoundaryExtractor();
    
    // Main extraction methods
    void extractBoundaries(const std::vector<Surface>& surfaces);
    void extractByNormal(const std::vector<Surface>& surfaces, double angleTolerance = 30.0);
    void extractByUserDefinition(const std::vector<Surface>& surfaces,
                                 const std::map<std::string, Vector3D>& normalDirections);
    
    // Boundary region access
    std::vector<BoundaryRegion> getBoundaryRegions() const { return regions; }
    int getNumRegions() const { return static_cast<int>(regions.size()); }
    const BoundaryRegion& getRegion(int index) const { return regions[index]; }
    
    // Automatic naming
    void autoNameRegions();
    void nameRegionByNormal(BoundaryRegion& region);
    
    // User-defined naming
    void setRegionName(int index, const std::string& name);
    void mergeRegions(const std::vector<int>& regionIndices, const std::string& newName);
    
    // Statistics
    void computeRegionStatistics();
    
    // Clear
    void clear() { regions.clear(); }
    
private:
    std::vector<BoundaryRegion> regions;
    
    // Helper methods
    bool normalsAreSimilar(const Vector3D& n1, const Vector3D& n2, double angleTolerance) const;
    std::string generateNameFromNormal(const Vector3D& normal) const;
    void assignTriangleToRegion(int triangleIndex, const Triangle& tri, int regionIndex);
};

} // namespace cfd
