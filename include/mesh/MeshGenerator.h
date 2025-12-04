#pragma once

#include "core/Mesh.h"
#include "geometry/GeometryReader.h"
#include <vector>
#include <memory>

namespace cfd {

/**
 * @brief Mesh generation parameters
 */
struct MeshParams {
    double baseSize = 1.0;
    double boundaryLayerHeight = 0.1;
    double growthRatio = 1.2;
    int numBoundaryLayers = 3;
    
    struct RefinementRegion {
        Vector3D center;
        double radius;
        double size;
    };
    std::vector<RefinementRegion> refinementRegions;
};

/**
 * @brief Mesh quality metrics
 */
struct MeshQualityMetrics {
    double minAngle = 0.0;
    double maxAngle = 0.0;
    double avgAngle = 0.0;
    double minAspectRatio = 0.0;
    double maxAspectRatio = 0.0;
    double avgAspectRatio = 0.0;
    double minSkewness = 0.0;
    double maxSkewness = 0.0;
    int numCells = 0;
    int numBadCells = 0;
};

/**
 * @brief Main mesh generator class
 */
class MeshGenerator {
public:
    MeshGenerator();
    ~MeshGenerator();
    
    // Configuration
    void setGeometry(const std::vector<Surface>& surfaces);
    void setMeshParameters(const MeshParams& params);
    
    // Generation
    bool generate();
    Mesh getMesh() const { return mesh; }
    
    // Quality assessment
    MeshQualityMetrics getQualityMetrics() const;
    
    // Status
    bool isGenerated() const { return generated; }
    std::string getLastError() const { return lastError; }
    
private:
    std::vector<Surface> surfaces;
    MeshParams params;
    Mesh mesh;
    bool generated;
    std::string lastError;
    
    // Internal generation steps
    bool generateSurfaceMesh();
    bool generateVolumeMesh();
    bool generateBoundaryLayers();
    bool refineRegions();
    
    // Helper methods
    void extractUniqueVertices();
    void buildCellConnectivity();
};

} // namespace cfd
