#include "geometry/STLReader.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstring>

namespace cfd {

STLReader::STLReader() {
}

bool STLReader::loadFile(const std::string& filename) {
    surfaces.clear();
    loaded = false;
    
    // Check if file exists
    std::ifstream test(filename);
    if (!test.good()) {
        return false;
    }
    test.close();
    
    // Determine if ASCII or binary
    bool success = false;
    if (isASCII(filename)) {
        success = loadASCII(filename);
    } else {
        success = loadBinary(filename);
    }
    
    if (success) {
        computeBounds();
        loaded = true;
    }
    
    return success;
}

bool STLReader::isASCII(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.good()) return false;
    
    // Read first 80 bytes
    char header[80];
    file.read(header, 80);
    
    // Check for "solid" keyword (ASCII STL starts with this)
    std::string headerStr(header, 80);
    std::transform(headerStr.begin(), headerStr.end(), headerStr.begin(), ::tolower);
    
    // ASCII files start with "solid"
    if (headerStr.find("solid") == 0) {
        // But binary files can also have "solid" in header
        // Check if rest of file looks like ASCII
        file.seekg(0);
        std::string line;
        std::getline(file, line);
        std::getline(file, line);
        
        // ASCII should have "facet" or "endsolid" on second line
        std::transform(line.begin(), line.end(), line.begin(), ::tolower);
        return (line.find("facet") != std::string::npos || 
                line.find("endsolid") != std::string::npos);
    }
    
    return false;
}

bool STLReader::loadASCII(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.good()) return false;
    
    Surface surface("default");
    std::string line;
    Triangle currentTriangle;
    Vector3D normal;
    int vertexCount = 0;
    
    while (std::getline(file, line)) {
        std::string keyword;
        std::vector<std::string> tokens;
        
        if (!parseASCIILine(line, keyword, tokens)) {
            continue;
        }
        
        if (keyword == "solid") {
            if (tokens.size() > 0) {
                surface.name = tokens[0];
            }
        } else if (keyword == "facet") {
            if (tokens.size() >= 4 && tokens[0] == "normal") {
                normal = Vector3D(
                    std::stod(tokens[1]),
                    std::stod(tokens[2]),
                    std::stod(tokens[3])
                );
            }
            vertexCount = 0;
        } else if (keyword == "vertex") {
            if (tokens.size() >= 3 && vertexCount < 3) {
                currentTriangle.vertices[vertexCount] = Vector3D(
                    std::stod(tokens[0]),
                    std::stod(tokens[1]),
                    std::stod(tokens[2])
                );
                vertexCount++;
            }
        } else if (keyword == "endfacet") {
            if (vertexCount == 3) {
                currentTriangle.normal = normal;
                surface.addTriangle(currentTriangle);
            }
        } else if (keyword == "endsolid") {
            if (surface.getNumTriangles() > 0) {
                surfaces.push_back(surface);
                surface = Surface("default");
            }
        }
    }
    
    // Add last surface if not empty
    if (surface.getNumTriangles() > 0) {
        surfaces.push_back(surface);
    }
    
    return surfaces.size() > 0;
}

bool STLReader::loadBinary(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.good()) return false;
    
    // Read 80-byte header
    char header[80];
    file.read(header, 80);
    
    // Read number of triangles
    uint32_t numTriangles = readUInt32(file);
    
    Surface surface("default");
    
    for (uint32_t i = 0; i < numTriangles; ++i) {
        Triangle tri;
        
        // Read normal
        tri.normal = readVector3(file);
        
        // Read vertices
        tri.vertices[0] = readVector3(file);
        tri.vertices[1] = readVector3(file);
        tri.vertices[2] = readVector3(file);
        
        // Read attribute byte count (unused)
        uint16_t attributeByteCount;
        file.read(reinterpret_cast<char*>(&attributeByteCount), sizeof(uint16_t));
        
        surface.addTriangle(tri);
    }
    
    surfaces.push_back(surface);
    
    return true;
}

bool STLReader::parseASCIILine(const std::string& line, std::string& keyword, 
                                std::vector<std::string>& tokens) {
    std::istringstream iss(line);
    std::string word;
    
    tokens.clear();
    
    // Read first word as keyword
    if (!(iss >> keyword)) {
        return false;
    }
    
    // Convert keyword to lowercase
    std::transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
    
    // Read remaining tokens
    while (iss >> word) {
        tokens.push_back(word);
    }
    
    return true;
}

Vector3D STLReader::parseVector(const std::vector<std::string>& tokens) {
    if (tokens.size() < 3) {
        return Vector3D(0, 0, 0);
    }
    return Vector3D(
        std::stod(tokens[0]),
        std::stod(tokens[1]),
        std::stod(tokens[2])
    );
}

uint32_t STLReader::readUInt32(std::ifstream& file) {
    uint32_t value;
    file.read(reinterpret_cast<char*>(&value), sizeof(uint32_t));
    return value;
}

float STLReader::readFloat(std::ifstream& file) {
    float value;
    file.read(reinterpret_cast<char*>(&value), sizeof(float));
    return value;
}

Vector3D STLReader::readVector3(std::ifstream& file) {
    float x = readFloat(file);
    float y = readFloat(file);
    float z = readFloat(file);
    return Vector3D(x, y, z);
}

bool STLReader::validate() const {
    if (!loaded || surfaces.empty()) {
        return false;
    }
    
    // Check that all surfaces have triangles
    for (const auto& surface : surfaces) {
        if (surface.getNumTriangles() == 0) {
            return false;
        }
        
        // Check for degenerate triangles
        for (const auto& tri : surface.triangles) {
            double area = tri.area();
            if (area < 1e-10) {
                return false;
            }
        }
    }
    
    return true;
}

} // namespace cfd
