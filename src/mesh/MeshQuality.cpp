#include "mesh/MeshQuality.h"
#include <sstream>
#include <iomanip>
#include <cmath>

namespace cfd {

MeshQuality::MeshQuality()
    : minAspectRatio(1.0), maxAspectRatio(1.0), avgAspectRatio(1.0),
      minSkewness(0.0), maxSkewness(0.0), avgSkewness(0.0),
      numBadCells(0), aspectRatioThreshold(10.0), skewnessThreshold(0.85) {
}

void MeshQuality::computeMetrics(const Mesh& mesh) {
    if (mesh.getNumCells() == 0) {
        return;
    }
    
    minAspectRatio = 1e10;
    maxAspectRatio = -1e10;
    double sumAspectRatio = 0.0;
    
    minSkewness = 1e10;
    maxSkewness = -1e10;
    double sumSkewness = 0.0;
    
    numBadCells = 0;
    
    for (int i = 0; i < mesh.getNumCells(); ++i) {
        double ar = computeCellAspectRatio(mesh, i);
        double skew = computeCellSkewness(mesh, i);
        
        minAspectRatio = std::min(minAspectRatio, ar);
        maxAspectRatio = std::max(maxAspectRatio, ar);
        sumAspectRatio += ar;
        
        minSkewness = std::min(minSkewness, skew);
        maxSkewness = std::max(maxSkewness, skew);
        sumSkewness += skew;
        
        if (ar > aspectRatioThreshold || skew > skewnessThreshold) {
            numBadCells++;
        }
    }
    
    avgAspectRatio = sumAspectRatio / mesh.getNumCells();
    avgSkewness = sumSkewness / mesh.getNumCells();
}

double MeshQuality::computeCellAspectRatio(const Mesh& mesh, int cellId) const {
    const Cell& cell = mesh.getCell(cellId);
    
    // Simplified aspect ratio: ratio of max to min edge length
    // In production, would compute actual aspect ratio based on cell shape
    
    double minEdge = 1e10;
    double maxEdge = -1e10;
    
    // Get all edges from faces
    for (int faceId : cell.faceIds) {
        const Face& face = mesh.getFace(faceId);
        for (size_t i = 0; i < face.nodeIds.size(); ++i) {
            size_t j = (i + 1) % face.nodeIds.size();
            const Vector3D& v1 = mesh.getNode(face.nodeIds[i]).position;
            const Vector3D& v2 = mesh.getNode(face.nodeIds[j]).position;
            double edgeLength = (v2 - v1).magnitude();
            
            minEdge = std::min(minEdge, edgeLength);
            maxEdge = std::max(maxEdge, edgeLength);
        }
    }
    
    return (minEdge > 1e-10) ? (maxEdge / minEdge) : 1.0;
}

double MeshQuality::computeCellSkewness(const Mesh& mesh, int cellId) const {
    const Cell& cell = mesh.getCell(cellId);
    
    // Simplified skewness computation
    // In production, would compute actual skewness based on cell geometry
    
    // Skewness = (optimal_volume - actual_volume) / optimal_volume
    // For now, return a placeholder value
    
    return 0.1;  // Low skewness (good quality)
}

double MeshQuality::computeFaceAngle(const Mesh& mesh, int faceId) const {
    const Face& face = mesh.getFace(faceId);
    
    if (face.nodeIds.size() < 3) {
        return 0.0;
    }
    
    // Compute angle at first vertex
    const Vector3D& v0 = mesh.getNode(face.nodeIds[0]).position;
    const Vector3D& v1 = mesh.getNode(face.nodeIds[1]).position;
    const Vector3D& v2 = mesh.getNode(face.nodeIds[face.nodeIds.size() - 1]).position;
    
    Vector3D e1 = (v1 - v0).normalized();
    Vector3D e2 = (v2 - v0).normalized();
    
    double cosAngle = e1.dot(e2);
    return std::acos(std::max(-1.0, std::min(1.0, cosAngle))) * 180.0 / M_PI;
}

std::string MeshQuality::generateReport() const {
    std::ostringstream oss;
    
    oss << "=== Mesh Quality Report ===\n";
    oss << std::fixed << std::setprecision(3);
    oss << "Aspect Ratio:\n";
    oss << "  Min: " << minAspectRatio << "\n";
    oss << "  Max: " << maxAspectRatio << "\n";
    oss << "  Avg: " << avgAspectRatio << "\n";
    oss << "\n";
    oss << "Skewness:\n";
    oss << "  Min: " << minSkewness << "\n";
    oss << "  Max: " << maxSkewness << "\n";
    oss << "  Avg: " << avgSkewness << "\n";
    oss << "\n";
    oss << "Bad Cells: " << numBadCells << "\n";
    
    return oss.str();
}

} // namespace cfd
