#pragma once

#include "core/Vector3D.h"
#include <vector>
#include <map>
#include <string>
#include <set>

namespace cfd {

/**
 * @brief Node (vertex) in the mesh
 */
struct Node {
    int id;
    Vector3D position;
    std::vector<int> connectedCells;  // Cells sharing this node
    
    Node() : id(-1) {}
    Node(int id_, const Vector3D& pos) : id(id_), position(pos) {}
};

/**
 * @brief Face connecting cells
 */
struct Face {
    int id;
    std::vector<int> nodeIds;
    int ownerCell;
    int neighborCell;  // -1 for boundary faces
    Vector3D normal;
    Vector3D centroid;
    double area;
    
    Face() : id(-1), ownerCell(-1), neighborCell(-1), area(0.0) {}
    
    bool isBoundary() const { return neighborCell == -1; }
};

/**
 * @brief Cell (control volume) in the mesh
 */
struct Cell {
    int id;
    std::vector<int> faceIds;
    std::vector<int> neighborCells;  // Neighboring cell IDs
    Vector3D centroid;
    double volume;
    
    Cell() : id(-1), volume(0.0) {}
    
    int getNumFaces() const { return static_cast<int>(faceIds.size()); }
    int getNumNeighbors() const { return static_cast<int>(neighborCells.size()); }
};

/**
 * @brief Boundary patch (collection of boundary faces)
 */
struct BoundaryPatch {
    std::string name;
    std::string type;  // "wall", "inlet", "outlet", "symmetry"
    std::vector<int> faceIds;
    
    BoundaryPatch() {}
    BoundaryPatch(const std::string& name_, const std::string& type_)
        : name(name_), type(type_) {}
};

/**
 * @brief Mesh class containing nodes, faces, cells, and connectivity
 */
class Mesh {
public:
    Mesh();
    ~Mesh();
    
    // Accessors
    int getNumNodes() const { return static_cast<int>(nodes.size()); }
    int getNumCells() const { return static_cast<int>(cells.size()); }
    int getNumFaces() const { return static_cast<int>(faces.size()); }
    int getNumBoundaryFaces() const;
    int getNumInternalFaces() const;
    
    const Node& getNode(int id) const { return nodes[id]; }
    const Cell& getCell(int id) const { return cells[id]; }
    const Face& getFace(int id) const { return faces[id]; }
    
    Node& getNode(int id) { return nodes[id]; }
    Cell& getCell(int id) { return cells[id]; }
    Face& getFace(int id) { return faces[id]; }
    
    // Connectivity queries
    std::vector<int> getCellNeighbors(int cellId) const;
    std::vector<int> getNodeCells(int nodeId) const;
    std::vector<int> getCellFaces(int cellId) const;
    
    // Geometric computations
    void computeFaceGeometry(int faceId);
    void computeCellGeometry(int cellId);
    void computeAllGeometry();
    
    // Mesh building
    int addNode(const Vector3D& position);
    int addCell(const std::vector<int>& faceIds);
    int addFace(const std::vector<int>& nodeIds, int owner, int neighbor = -1);
    void addBoundaryPatch(const std::string& name, const std::string& type);
    void assignFaceToBoundary(int faceId, const std::string& patchName);
    
    // Connectivity building
    void buildConnectivity();
    void buildCellNeighbors();
    void buildNodeCellConnectivity();
    
    // Validation
    bool validate() const;
    
    // Data members
    std::vector<Node> nodes;
    std::vector<Cell> cells;
    std::vector<Face> faces;
    std::map<std::string, BoundaryPatch> boundaries;
    
private:
    // Helper methods
    Vector3D computeFaceCentroid(const Face& face) const;
    Vector3D computeFaceNormal(const Face& face) const;
    double computeFaceArea(const Face& face) const;
    Vector3D computeCellCentroid(const Cell& cell) const;
    double computeCellVolume(const Cell& cell) const;
};

} // namespace cfd
