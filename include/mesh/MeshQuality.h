#pragma once

#include "core/Mesh.h"
#include <vector>

namespace cfd {

/**
 * @brief Mesh quality assessment
 */
class MeshQuality {
public:
    MeshQuality();
    
    // Quality metrics computation
    void computeMetrics(const Mesh& mesh);
    
    // Individual metrics
    double computeCellAspectRatio(const Mesh& mesh, int cellId) const;
    double computeCellSkewness(const Mesh& mesh, int cellId) const;
    double computeFaceAngle(const Mesh& mesh, int faceId) const;
    
    // Statistics
    double getMinAspectRatio() const { return minAspectRatio; }
    double getMaxAspectRatio() const { return maxAspectRatio; }
    double getAvgAspectRatio() const { return avgAspectRatio; }
    
    double getMinSkewness() const { return minSkewness; }
    double getMaxSkewness() const { return maxSkewness; }
    double getAvgSkewness() const { return avgSkewness; }
    
    int getNumBadCells() const { return numBadCells; }
    
    // Quality thresholds
    void setAspectRatioThreshold(double threshold) { aspectRatioThreshold = threshold; }
    void setSkewnessThreshold(double threshold) { skewnessThreshold = threshold; }
    
    // Report generation
    std::string generateReport() const;
    
private:
    double minAspectRatio, maxAspectRatio, avgAspectRatio;
    double minSkewness, maxSkewness, avgSkewness;
    int numBadCells;
    
    double aspectRatioThreshold;
    double skewnessThreshold;
};

} // namespace cfd
