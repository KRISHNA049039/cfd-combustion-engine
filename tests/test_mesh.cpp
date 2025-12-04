#include <gtest/gtest.h>
#include "core/Mesh.h"

using namespace cfd;

TEST(MeshTest, Construction) {
    Mesh mesh;
    EXPECT_EQ(mesh.getNumNodes(), 0);
    EXPECT_EQ(mesh.getNumCells(), 0);
    EXPECT_EQ(mesh.getNumFaces(), 0);
}

TEST(MeshTest, AddNodes) {
    Mesh mesh;
    int id0 = mesh.addNode(Vector3D(0, 0, 0));
    int id1 = mesh.addNode(Vector3D(1, 0, 0));
    int id2 = mesh.addNode(Vector3D(0, 1, 0));
    
    EXPECT_EQ(mesh.getNumNodes(), 3);
    EXPECT_EQ(id0, 0);
    EXPECT_EQ(id1, 1);
    EXPECT_EQ(id2, 2);
    EXPECT_DOUBLE_EQ(mesh.getNode(0).position.x, 0.0);
    EXPECT_DOUBLE_EQ(mesh.getNode(1).position.x, 1.0);
    EXPECT_DOUBLE_EQ(mesh.getNode(2).position.y, 1.0);
}

TEST(MeshTest, AddFaces) {
    Mesh mesh;
    mesh.addNode(Vector3D(0, 0, 0));
    mesh.addNode(Vector3D(1, 0, 0));
    mesh.addNode(Vector3D(1, 1, 0));
    mesh.addNode(Vector3D(0, 1, 0));
    
    std::vector<int> nodeIds = {0, 1, 2, 3};
    int faceId = mesh.addFace(nodeIds, 0, -1);
    
    EXPECT_EQ(mesh.getNumFaces(), 1);
    EXPECT_EQ(faceId, 0);
    EXPECT_TRUE(mesh.getFace(0).isBoundary());
    EXPECT_EQ(mesh.getFace(0).ownerCell, 0);
}

TEST(MeshTest, FaceGeometry) {
    Mesh mesh;
    mesh.addNode(Vector3D(0, 0, 0));
    mesh.addNode(Vector3D(1, 0, 0));
    mesh.addNode(Vector3D(1, 1, 0));
    mesh.addNode(Vector3D(0, 1, 0));
    
    std::vector<int> nodeIds = {0, 1, 2, 3};
    int faceId = mesh.addFace(nodeIds, 0, -1);
    
    mesh.computeFaceGeometry(faceId);
    
    const Face& face = mesh.getFace(faceId);
    EXPECT_NEAR(face.centroid.x, 0.5, 1e-10);
    EXPECT_NEAR(face.centroid.y, 0.5, 1e-10);
    EXPECT_NEAR(face.area, 1.0, 1e-10);
}

TEST(MeshTest, BoundaryPatches) {
    Mesh mesh;
    mesh.addBoundaryPatch("inlet", "inlet");
    mesh.addBoundaryPatch("outlet", "outlet");
    mesh.addBoundaryPatch("walls", "wall");
    
    EXPECT_EQ(mesh.boundaries.size(), 3);
    EXPECT_EQ(mesh.boundaries["inlet"].type, "inlet");
    EXPECT_EQ(mesh.boundaries["walls"].type, "wall");
}

TEST(MeshTest, Connectivity) {
    Mesh mesh;
    
    // Create a simple 2-cell mesh
    mesh.addNode(Vector3D(0, 0, 0));
    mesh.addNode(Vector3D(1, 0, 0));
    mesh.addNode(Vector3D(2, 0, 0));
    mesh.addNode(Vector3D(0, 1, 0));
    mesh.addNode(Vector3D(1, 1, 0));
    mesh.addNode(Vector3D(2, 1, 0));
    
    // Face between cells
    int face0 = mesh.addFace({0, 1, 4, 3}, 0, -1);
    int face1 = mesh.addFace({1, 2, 5, 4}, 1, -1);
    int face2 = mesh.addFace({1, 4}, 0, 1);  // Shared face
    
    int cell0 = mesh.addCell({face0, face2});
    int cell1 = mesh.addCell({face1, face2});
    
    mesh.buildConnectivity();
    
    EXPECT_EQ(mesh.getNumCells(), 2);
    
    auto neighbors0 = mesh.getCellNeighbors(0);
    auto neighbors1 = mesh.getCellNeighbors(1);
    
    EXPECT_EQ(neighbors0.size(), 1);
    EXPECT_EQ(neighbors1.size(), 1);
    EXPECT_EQ(neighbors0[0], 1);
    EXPECT_EQ(neighbors1[0], 0);
}

TEST(MeshTest, Validation) {
    Mesh mesh;
    mesh.addNode(Vector3D(0, 0, 0));
    mesh.addNode(Vector3D(1, 0, 0));
    mesh.addNode(Vector3D(0, 1, 0));
    
    std::vector<int> nodeIds = {0, 1, 2};
    mesh.addFace(nodeIds, 0, -1);
    mesh.addCell({0});
    
    EXPECT_TRUE(mesh.validate());
}
