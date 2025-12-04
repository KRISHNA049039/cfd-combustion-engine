#include "geometry/BoundaryExtractor.h"
#include <cmath>
#include <algorithm>
#include <sstream>

namespace cfd {

BoundaryExtractor::BoundaryExtractor() {
}

void BoundaryExtractor::extractBoundaries(const std::vector<Surface>& surfaces) {
    // Default: extract by normal with 30 degree tolerance
    extractByNormal(surfaces, 30.0);
}

void BoundaryExtractor::extractByNormal(const std::vector<Surface>& surfaces, double angleTolerance) {
    clear();
    
    int triangleIndex = 0;
    
    for (const auto& surface : surfaces) {
        for (const auto& tri : surface.triangles) {
            bool assigned = false;
            
            // Try to assign to existing region
            for (size_t i = 0; i < regions.size(); ++i) {
                if (normalsAreSimilar(tri.normal, regions[i].averageNormal, angleTolerance)) {
                    assignTriangleToRegion(triangleIndex, tri, i);
                    assigned = true;
                    break;
                }
            }
            
            // Create new region if not assigned
            if (!assigned) {
                BoundaryRegion newRegion;
                newRegion.averageNormal = tri.normal;
                regions.push_back(newRegion);
                assignTriangleToRegion(triangleIndex, tri, regions.size() - 1);
            }
            
            triangleIndex++;
        }
    }
    
    // Compute statistics and auto-name
    computeRegionStatistics();
    autoNameRegions();
}

void BoundaryExtractor::extractByUserDefinition(const std::vector<Surface>& surfaces,
                                                const std::map<std::string, Vector3D>& normalDirections) {
    clear();
    
    // Create regions based on user definitions
    for (const auto& pair : normalDirections) {
        BoundaryRegion region(pair.first);
        region.averageNormal = pair.second.normalized();
        regions.push_back(region);
    }
    
    // Assign triangles to regions
    int triangleIndex = 0;
    double angleTolerance = 30.0;  // Default tolerance
    
    for (const auto& surface : surfaces) {
        for (const auto& tri : surface.triangles) {
            // Find best matching region
            int bestRegion = -1;
            double bestDot = -1.0;
            
            for (size_t i = 0; i < regions.size(); ++i) {
                double dot = tri.normal.dot(regions[i].averageNormal);
                if (dot > bestDot) {
                    bestDot = dot;
                    bestRegion = i;
                }
            }
            
            if (bestRegion >= 0 && normalsAreSimilar(tri.normal, regions[bestRegion].averageNormal, angleTolerance)) {
                assignTriangleToRegion(triangleIndex, tri, bestRegion);
            }
            
            triangleIndex++;
        }
    }
    
    computeRegionStatistics();
}

void BoundaryExtractor::assignTriangleToRegion(int triangleIndex, const Triangle& tri, int regionIndex) {
    regions[regionIndex].triangleIndices.push_back(triangleIndex);
    regions[regionIndex].totalArea += tri.area();
}

void BoundaryExtractor::computeRegionStatistics() {
    // This would need access to the actual triangles to compute centroids
    // For now, just normalize the average normals
    for (auto& region : regions) {
        region.averageNormal.normalize();
    }
}

void BoundaryExtractor::autoNameRegions() {
    for (auto& region : regions) {
        if (region.name.empty()) {
            nameRegionByNormal(region);
        }
    }
}

void BoundaryExtractor::nameRegionByNormal(BoundaryRegion& region) {
    region.name = generateNameFromNormal(region.averageNormal);
}

std::string BoundaryExtractor::generateNameFromNormal(const Vector3D& normal) const {
    // Determine primary direction
    double absX = std::abs(normal.x);
    double absY = std::abs(normal.y);
    double absZ = std::abs(normal.z);
    
    std::string name;
    
    if (absX > absY && absX > absZ) {
        name = (normal.x > 0) ? "x_positive" : "x_negative";
    } else if (absY > absX && absY > absZ) {
        name = (normal.y > 0) ? "y_positive" : "y_negative";
    } else {
        name = (normal.z > 0) ? "z_positive" : "z_negative";
    }
    
    return name;
}

bool BoundaryExtractor::normalsAreSimilar(const Vector3D& n1, const Vector3D& n2, 
                                         double angleTolerance) const {
    double dot = n1.dot(n2);
    double angleRad = std::acos(std::max(-1.0, std::min(1.0, dot)));
    double angleDeg = angleRad * 180.0 / M_PI;
    
    return angleDeg < angleTolerance;
}

void BoundaryExtractor::setRegionName(int index, const std::string& name) {
    if (index >= 0 && index < static_cast<int>(regions.size())) {
        regions[index].name = name;
    }
}

void BoundaryExtractor::mergeRegions(const std::vector<int>& regionIndices, 
                                    const std::string& newName) {
    if (regionIndices.empty()) return;
    
    // Create merged region
    BoundaryRegion merged(newName);
    
    for (int idx : regionIndices) {
        if (idx >= 0 && idx < static_cast<int>(regions.size())) {
            const auto& region = regions[idx];
            merged.triangleIndices.insert(merged.triangleIndices.end(),
                                         region.triangleIndices.begin(),
                                         region.triangleIndices.end());
            merged.totalArea += region.totalArea;
        }
    }
    
    // Remove old regions (in reverse order to maintain indices)
    std::vector<int> sortedIndices = regionIndices;
    std::sort(sortedIndices.rbegin(), sortedIndices.rend());
    
    for (int idx : sortedIndices) {
        if (idx >= 0 && idx < static_cast<int>(regions.size())) {
            regions.erase(regions.begin() + idx);
        }
    }
    
    // Add merged region
    regions.push_back(merged);
}

} // namespace cfd
