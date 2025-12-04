#pragma once

#include "core/Vector3D.h"
#include "core/FieldManager.h"

namespace cfd {

struct SparkConfig {
    Vector3D location;
    double ignitionTime = 0.001;
    double ignitionEnergy = 50.0;  // millijoules
    double kernelRadius = 0.5;     // mm
};

class SparkIgnition {
public:
    SparkIgnition();
    
    void initialize(const SparkConfig& config);
    void apply(FieldManager& fields, double time, const class Mesh& mesh);
    
    bool isActive(double time) const;
    Vector3D getLocation() const { return config.location; }
    
private:
    SparkConfig config;
    bool applied;
    
    void depositEnergy(FieldManager& fields, const class Mesh& mesh);
};

} // namespace cfd
