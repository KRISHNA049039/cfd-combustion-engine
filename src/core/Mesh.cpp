#include "core/Mesh.h"
#include <algorithm>
#include <stdexcept>
#include <cmath>

namespace cfd {

Mesh::Mesh() {
}

Mesh::~Mesh() {
}

int Mesh::getNumBoundaryFaces() const {
    int count = 0;
    for (const auto& face : faces) {
        if (face.isBoundary()) count++;
    }
    return count;
}

int Mesh::getNumInternalFaces() const {
    return getNumFaces() - getNumBoundaryFaces();
}

std::vector<int> Mesh::getCellNeighbors(int cellId) const {
    if (cellId < 0 || cellId >= getNumCells()) {
        throw std::out_of_range("Cell ID out of range");
    }
    return cells[cellId].neighborCells;
}

std::vector<int> Mesh::getNodeCells(int nodeId) const {
    if (nodeId < 0 || nodeId >= getNumNodes()) {
        throw std::out_of_range("Node ID out of range");
    }
    return nodes[nodeId].connectedCells;
}

std::vector<int> Mesh::getCellFaces(int cellId) const {
    if (cellId < 0 || cellId >= getNumCells()) {
        throw std::out_of_range("Cell ID out of range");
    }
    return cells[cellId].faceIds;
}

int Mesh::addNode(const Vector3D& position) {
    int id = static_cast<int>(nodes.size());
    nodes.push_back(Node(id, position));
    return id;
}

int Mesh::addCell(const std::vector<int>& faceIds) {
    int id = static_cast<int>(cells.size());
    Cell cell;
    cell.id = id;
    cell.faceIds = faceIds;
    cells.push_back(cell);
    return id;
}

int Mesh::addFace(const std::vector<int>& nodeIds, int owner, int neighbor) {
    int id = static_cast<int>(faces.size());
    Face face;
    face.id = id;
    face.nodeIds = nodeIds;
    face.ownerCell = owner;
    face.neighborCell = neighbor;
    faces.push_back(face);
    return id;
}

void Mesh::addBoundaryPatch(const std::string& name, const std::string& type) {
    boundaries[name] = BoundaryPatch(name, type);
}

void Mesh::assignFaceToBoundary(int faceId, const std::string& patchName) {
    if (boundaries.find(patchName) == boundaries.end()) {
        throw std::runtime_error("Boundary patch not found: " + patchName);
    }
    boundaries[patchName].faceIds.push_back(faceId);
}

Vector3D Mesh::computeFaceCentroid(const Face& face) const {
    Vector3D centroid(0, 0, 0);
    for (int nodeId : face.nodeIds) {
        centroid += nodes[nodeId].position;
    }
    return centroid / static_cast<double>(face.nodeIds.size());
}

Vector3D Mesh::computeFaceNormal(const Face& face) const {
    if (face.nodeIds.size() < 3) {
        return Vector3D(0, 0, 0);
    }
    
    // Use first three nodes to compute normal
    const Vector3D& p0 = nodes[face.nodeIds[0]].position;
    const Vector3D& p1 = nodes[face.nodeIds[1]].position;
    const Vector3D& p2 = nodes[face.nodeIds[2]].position;
    
    Vector3D v1 = p1 - p0;
    Vector3D v2 = p2 - p0;
    
    return v1.cross(v2).normalized();
}

double Mesh::computeFaceArea(const Face& face) const {
    if (face.nodeIds.size() < 3) {
        return 0.0;
    }
    
    // Triangulate face and sum areas
    double totalArea = 0.0;
    const Vector3D& p0 = nodes[face.nodeIds[0]].position;
    
    for (size_t i = 1; i < face.nodeIds.size() - 1; ++i) {
        const Vector3D& p1 = nodes[face.nodeIds[i]].position;
        const Vector3D& p2 = nodes[face.nodeIds[i + 1]].position;
        
        Vector3D v1 = p1 - p0;
        Vector3D v2 = p2 - p0;
        totalArea += 0.5 * v1.cross(v2).magnitude();
    }
    
    return totalArea;
}

void Mesh::computeFaceGeometry(int faceId) {
    Face& face = faces[faceId];
    face.centroid = computeFaceCentroid(face);
    face.normal = computeFaceNormal(face);
    face.area = computeFaceArea(face);
}

Vector3D Mesh::computeCellCentroid(const Cell& cell) const {
    Vector3D centroid(0, 0, 0);
    int nodeCount = 0;
    
    std::set<int> uniqueNodes;
    for (int faceId : cell.faceIds) {
        const Face& face = faces[faceId];
        for (int nodeId : face.nodeIds) {
            uniqueNodes.insert(nodeId);
        }
    }
    
    for (int nodeId : uniqueNodes) {
        centroid += nodes[nodeId].position;
        nodeCount++;
    }
    
    return centroid / static_cast<double>(nodeCount);
}

double Mesh::computeCellVolume(const Cell& cell) const {
    // Approximate volume using divergence theorem
    // V = (1/3) * sum(face_area * distance_to_centroid)
    double volume = 0.0;
    Vector3D cellCenter = cell.centroid;
    
    for (int faceId : cell.faceIds) {
        const Face& face = faces[faceId];
        Vector3D r = face.centroid - cellCenter;
        double contribution = face.area * r.dot(face.normal);
        volume += contribution;
    }
    
    return std::abs(volume / 3.0);
}

void Mesh::computeCellGeometry(int cellId) {
    Cell& cell = cells[cellId];
    cell.centroid = computeCellCentroid(cell);
    cell.volume = computeCellVolume(cell);
}

void Mesh::computeAllGeometry() {
    // Compute face geometry first
    for (int i = 0; i < getNumFaces(); ++i) {
        computeFaceGeometry(i);
    }
    
    // Then compute cell geometry
    for (int i = 0; i < getNumCells(); ++i) {
        computeCellGeometry(i);
    }
}

void Mesh::buildCellNeighbors() {
    // Clear existing connectivity
    for (auto& cell : cells) {
        cell.neighborCells.clear();
    }
    
    // Build neighbor lists from faces
    for (const auto& face : faces) {
        if (face.ownerCell >= 0 && face.neighborCell >= 0) {
            cells[face.ownerCell].neighborCells.push_back(face.neighborCell);
            cells[face.neighborCell].neighborCells.push_back(face.ownerCell);
        }
    }
}

void Mesh::buildNodeCellConnectivity() {
    // Clear existing connectivity
    for (auto& node : nodes) {
        node.connectedCells.clear();
    }
    
    // Build node-to-cell connectivity
    for (int cellId = 0; cellId < getNumCells(); ++cellId) {
        const Cell& cell = cells[cellId];
        
        std::set<int> uniqueNodes;
        for (int faceId : cell.faceIds) {
            const Face& face = faces[faceId];
            for (int nodeId : face.nodeIds) {
                uniqueNodes.insert(nodeId);
            }
        }
        
        for (int nodeId : uniqueNodes) {
            nodes[nodeId].connectedCells.push_back(cellId);
        }
    }
}

void Mesh::buildConnectivity() {
    buildCellNeighbors();
    buildNodeCellConnectivity();
}

bool Mesh::validate() const {
    // Check for valid node IDs in faces
    for (const auto& face : faces) {
        for (int nodeId : face.nodeIds) {
            if (nodeId < 0 || nodeId >= getNumNodes()) {
                return false;
            }
        }
        
        // Check owner/neighbor cells
        if (face.ownerCell < 0 || face.ownerCell >= getNumCells()) {
            return false;
        }
        if (face.neighborCell >= getNumCells()) {
            return false;
        }
    }
    
    // Check for valid face IDs in cells
    for (const auto& cell : cells) {
        for (int faceId : cell.faceIds) {
            if (faceId < 0 || faceId >= getNumFaces()) {
                return false;
            }
        }
    }
    
    return true;
}

} // namespace cfd
