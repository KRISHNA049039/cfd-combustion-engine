#include <gtest/gtest.h>
#include "geometry/GeometryReader.h"
#include "geometry/STLReader.h"
#include "geometry/GeometryValidator.h"
#include "geometry/BoundaryExtractor.h"
#include <fstream>
#include <cmath>

using namespace cfd;

// Helper function to create a simple cube STL file
void createCubeSTL(const std::string& filename, bool binary = false) {
    if (binary) {
        // Binary STL format
        std::ofstream file(filename, std::ios::binary);
        
        // Header (80 bytes)
        char header[80] = {0};
        file.write(header, 80);
        
        // Number of triangles (12 for a cube)
        uint32_t numTriangles = 12;
        file.write(reinterpret_cast<const char*>(&numTriangles), sizeof(uint32_t));
        
        // Define cube vertices
        float vertices[8][3] = {
            {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0},  // Bottom
            {0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}   // Top
        };
        
        // Define triangles (2 per face, 6 faces)
        int triangles[12][3] = {
            {0, 1, 2}, {0, 2, 3},  // Bottom
            {4, 6, 5}, {4, 7, 6},  // Top
            {0, 4, 5}, {0, 5, 1},  // Front
            {2, 6, 7}, {2, 7, 3},  // Back
            {0, 3, 7}, {0, 7, 4},  // Left
            {1, 5, 6}, {1, 6, 2}   // Right
        };
        
        for (int i = 0; i < 12; ++i) {
            // Normal (placeholder)
            float normal[3] = {0, 0, 1};
            file.write(reinterpret_cast<const char*>(normal), 3 * sizeof(float));
            
            // Vertices
            for (int j = 0; j < 3; ++j) {
                int vIdx = triangles[i][j];
                file.write(reinterpret_cast<const char*>(vertices[vIdx]), 3 * sizeof(float));
            }
            
            // Attribute byte count
            uint16_t attr = 0;
            file.write(reinterpret_cast<const char*>(&attr), sizeof(uint16_t));
        }
        
        file.close();
    } else {
        // ASCII STL format
        std::ofstream file(filename);
        file << "solid cube\n";
        
        // Bottom face
        file << "  facet normal 0 0 -1\n";
        file << "    outer loop\n";
        file << "      vertex 0 0 0\n";
        file << "      vertex 1 0 0\n";
        file << "      vertex 1 1 0\n";
        file << "    endloop\n";
        file << "  endfacet\n";
        
        file << "  facet normal 0 0 -1\n";
        file << "    outer loop\n";
        file << "      vertex 0 0 0\n";
        file << "      vertex 1 1 0\n";
        file << "      vertex 0 1 0\n";
        file << "    endloop\n";
        file << "  endfacet\n";
        
        // Top face
        file << "  facet normal 0 0 1\n";
        file << "    outer loop\n";
        file << "      vertex 0 0 1\n";
        file << "      vertex 1 1 1\n";
        file << "      vertex 1 0 1\n";
        file << "    endloop\n";
        file << "  endfacet\n";
        
        file << "  facet normal 0 0 1\n";
        file << "    outer loop\n";
        file << "      vertex 0 0 1\n";
        file << "      vertex 0 1 1\n";
        file << "      vertex 1 1 1\n";
        file << "    endloop\n";
        file << "  endfacet\n";
        
        file << "endsolid cube\n";
        file.close();
    }
}

TEST(TriangleTest, Construction) {
    Triangle tri(Vector3D(0, 0, 0), Vector3D(1, 0, 0), Vector3D(0, 1, 0));
    
    EXPECT_DOUBLE_EQ(tri.vertices[0].x, 0.0);
    EXPECT_DOUBLE_EQ(tri.vertices[1].x, 1.0);
    EXPECT_DOUBLE_EQ(tri.vertices[2].y, 1.0);
}

TEST(TriangleTest, NormalComputation) {
    Triangle tri(Vector3D(0, 0, 0), Vector3D(1, 0, 0), Vector3D(0, 1, 0));
    
    // Normal should point in +Z direction
    EXPECT_NEAR(tri.normal.x, 0.0, 1e-10);
    EXPECT_NEAR(tri.normal.y, 0.0, 1e-10);
    EXPECT_NEAR(tri.normal.z, 1.0, 1e-10);
}

TEST(TriangleTest, AreaComputation) {
    Triangle tri(Vector3D(0, 0, 0), Vector3D(1, 0, 0), Vector3D(0, 1, 0));
    
    double area = tri.area();
    EXPECT_NEAR(area, 0.5, 1e-10);
}

TEST(TriangleTest, Centroid) {
    Triangle tri(Vector3D(0, 0, 0), Vector3D(3, 0, 0), Vector3D(0, 3, 0));
    
    Vector3D centroid = tri.centroid();
    EXPECT_NEAR(centroid.x, 1.0, 1e-10);
    EXPECT_NEAR(centroid.y, 1.0, 1e-10);
    EXPECT_NEAR(centroid.z, 0.0, 1e-10);
}

TEST(BoundingBoxTest, Expansion) {
    BoundingBox bbox;
    
    bbox.expand(Vector3D(0, 0, 0));
    bbox.expand(Vector3D(1, 2, 3));
    bbox.expand(Vector3D(-1, -2, -3));
    
    EXPECT_DOUBLE_EQ(bbox.min.x, -1.0);
    EXPECT_DOUBLE_EQ(bbox.min.y, -2.0);
    EXPECT_DOUBLE_EQ(bbox.min.z, -3.0);
    EXPECT_DOUBLE_EQ(bbox.max.x, 1.0);
    EXPECT_DOUBLE_EQ(bbox.max.y, 2.0);
    EXPECT_DOUBLE_EQ(bbox.max.z, 3.0);
}

TEST(BoundingBoxTest, CenterAndSize) {
    BoundingBox bbox;
    bbox.expand(Vector3D(0, 0, 0));
    bbox.expand(Vector3D(2, 4, 6));
    
    Vector3D center = bbox.center();
    EXPECT_DOUBLE_EQ(center.x, 1.0);
    EXPECT_DOUBLE_EQ(center.y, 2.0);
    EXPECT_DOUBLE_EQ(center.z, 3.0);
    
    Vector3D size = bbox.size();
    EXPECT_DOUBLE_EQ(size.x, 2.0);
    EXPECT_DOUBLE_EQ(size.y, 4.0);
    EXPECT_DOUBLE_EQ(size.z, 6.0);
}

TEST(STLReaderTest, LoadASCII) {
    createCubeSTL("test_cube_ascii.stl", false);
    
    STLReader reader;
    bool success = reader.loadFile("test_cube_ascii.stl");
    
    EXPECT_TRUE(success);
    EXPECT_GT(reader.getNumTriangles(), 0);
    
    auto surfaces = reader.getSurfaces();
    EXPECT_GT(surfaces.size(), 0);
    
    // Clean up
    std::remove("test_cube_ascii.stl");
}

TEST(STLReaderTest, LoadBinary) {
    createCubeSTL("test_cube_binary.stl", true);
    
    STLReader reader;
    bool success = reader.loadFile("test_cube_binary.stl");
    
    EXPECT_TRUE(success);
    EXPECT_EQ(reader.getNumTriangles(), 12);
    
    // Clean up
    std::remove("test_cube_binary.stl");
}

TEST(STLReaderTest, BoundingBox) {
    createCubeSTL("test_cube.stl", false);
    
    STLReader reader;
    reader.loadFile("test_cube.stl");
    
    BoundingBox bbox = reader.getBounds();
    
    EXPECT_NEAR(bbox.min.x, 0.0, 1e-6);
    EXPECT_NEAR(bbox.min.y, 0.0, 1e-6);
    EXPECT_NEAR(bbox.min.z, 0.0, 1e-6);
    EXPECT_NEAR(bbox.max.x, 1.0, 1e-6);
    EXPECT_NEAR(bbox.max.y, 1.0, 1e-6);
    EXPECT_NEAR(bbox.max.z, 1.0, 1e-6);
    
    // Clean up
    std::remove("test_cube.stl");
}

TEST(STLReaderTest, Scale) {
    createCubeSTL("test_cube.stl", false);
    
    STLReader reader;
    reader.loadFile("test_cube.stl");
    reader.scale(2.0);
    
    BoundingBox bbox = reader.getBounds();
    
    EXPECT_NEAR(bbox.max.x, 2.0, 1e-6);
    EXPECT_NEAR(bbox.max.y, 2.0, 1e-6);
    EXPECT_NEAR(bbox.max.z, 2.0, 1e-6);
    
    // Clean up
    std::remove("test_cube.stl");
}

TEST(GeometryValidatorTest, DegenerateTriangles) {
    GeometryValidator validator;
    
    // Create surface with degenerate triangle
    Surface surface;
    Triangle degTri(Vector3D(0, 0, 0), Vector3D(0, 0, 0), Vector3D(1, 0, 0));
    surface.addTriangle(degTri);
    
    std::vector<Surface> surfaces = {surface};
    
    bool valid = validator.checkDegenerateTriangles(surfaces);
    EXPECT_FALSE(valid);
    EXPECT_GT(validator.getNumErrors(), 0);
}

TEST(GeometryValidatorTest, ValidTriangles) {
    GeometryValidator validator;
    
    Surface surface;
    Triangle tri(Vector3D(0, 0, 0), Vector3D(1, 0, 0), Vector3D(0, 1, 0));
    surface.addTriangle(tri);
    
    std::vector<Surface> surfaces = {surface};
    
    bool valid = validator.checkDegenerateTriangles(surfaces);
    EXPECT_TRUE(valid);
    EXPECT_EQ(validator.getNumErrors(), 0);
}

TEST(BoundaryExtractorTest, ExtractByNormal) {
    BoundaryExtractor extractor;
    
    // Create surfaces with different normals
    Surface surface;
    
    // Bottom face (normal pointing down)
    Triangle tri1(Vector3D(0, 0, 0), Vector3D(1, 0, 0), Vector3D(1, 1, 0));
    tri1.normal = Vector3D(0, 0, -1);
    surface.addTriangle(tri1);
    
    // Top face (normal pointing up)
    Triangle tri2(Vector3D(0, 0, 1), Vector3D(1, 1, 1), Vector3D(1, 0, 1));
    tri2.normal = Vector3D(0, 0, 1);
    surface.addTriangle(tri2);
    
    std::vector<Surface> surfaces = {surface};
    
    extractor.extractByNormal(surfaces, 30.0);
    
    EXPECT_EQ(extractor.getNumRegions(), 2);
}

TEST(BoundaryExtractorTest, AutoNaming) {
    BoundaryExtractor extractor;
    
    Surface surface;
    
    // Create triangle with normal pointing in +X direction
    Triangle tri(Vector3D(0, 0, 0), Vector3D(0, 1, 0), Vector3D(0, 0, 1));
    tri.normal = Vector3D(1, 0, 0);
    surface.addTriangle(tri);
    
    std::vector<Surface> surfaces = {surface};
    
    extractor.extractByNormal(surfaces);
    
    EXPECT_GT(extractor.getNumRegions(), 0);
    
    const auto& region = extractor.getRegion(0);
    EXPECT_FALSE(region.name.empty());
}
