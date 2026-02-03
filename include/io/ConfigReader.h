#pragma once

#include <array>
#include <string>
#include <vector>

namespace cfd {

struct SparkConfig {
    std::array<double, 3> location{0.0, 0.0, 0.0};
    double time = 0.0;
    double energy = 0.0;
};

struct CombustionConfig {
    std::string mechanism;
    double ethanolFraction = 0.0;
    SparkConfig spark;
};

struct GeometryConfig {
    std::string file;
    double scale = 1.0;
};

struct MeshConfig {
    double baseSize = 0.5;
    int boundaryLayers = 0;
};

struct SimulationTimingConfig {
    double startTime = 0.0;
    double endTime = 0.0;
    double timeStep = 1e-6;
    double outputInterval = 1e-4;
};

struct EngineConfig {
    GeometryConfig geometry;
    MeshConfig mesh;
    SimulationTimingConfig simulation;
    CombustionConfig combustion;
};

class ConfigReader {
public:
    static ConfigReader fromFile(const std::string& path);

    const EngineConfig& config() const { return config_; }
    const std::vector<std::string>& warnings() const { return warnings_; }
    const std::vector<std::string>& errors() const { return errors_; }

    bool isValid() const { return errors_.empty(); }
    std::string summarize() const;

private:
    EngineConfig config_{};
    std::vector<std::string> warnings_{};
    std::vector<std::string> errors_{};
};

} // namespace cfd
