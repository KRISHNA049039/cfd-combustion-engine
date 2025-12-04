#pragma once

#include "geometry/GeometryReader.h"
#include <fstream>

namespace cfd {

/**
 * @brief STL file reader (ASCII and binary formats)
 */
class STLReader : public GeometryReader {
public:
    STLReader();
    ~STLReader() override = default;
    
    bool loadFile(const std::string& filename) override;
    std::vector<Surface> getSurfaces() const override { return surfaces; }
    BoundingBox getBounds() const override { return bounds; }
    bool validate() const override;
    
private:
    bool isASCII(const std::string& filename);
    bool loadASCII(const std::string& filename);
    bool loadBinary(const std::string& filename);
    
    // ASCII parsing helpers
    bool parseASCIILine(const std::string& line, std::string& keyword, std::vector<std::string>& tokens);
    Vector3D parseVector(const std::vector<std::string>& tokens);
    
    // Binary parsing helpers
    uint32_t readUInt32(std::ifstream& file);
    float readFloat(std::ifstream& file);
    Vector3D readVector3(std::ifstream& file);
};

} // namespace cfd
