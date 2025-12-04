#pragma once

#include "core/Mesh.h"
#include "core/FieldManager.h"
#include "combustion/SparkIgnition.h"
#include "combustion/FlameTracker.h"
#include "combustion/LaminarFlameSpeed.h"

namespace cfd {

struct CombustionConfig {
    std::string model = "flamelet";
    double equivalenceRatio = 1.0;
    bool enableSpark = true;
};

class CombustionModel {
public:
    CombustionModel();
    
    void initialize(const CombustionConfig& config);
    void initializeSpark(const SparkConfig& spark, double time);
    void solve(FieldManager& fields, double dt);
    void updateFlamePosition(FieldManager& fields, double dt);
    
    double getHeatReleaseRate() const { return heatReleaseRate; }
    double getBurnedMassFraction() const { return burnedMassFraction; }
    
private:
    CombustionConfig config;
    SparkIgnition sparkIgnition;
    FlameTracker flameTracker;
    LaminarFlameSpeed flameSpeed;
    
    double heatReleaseRate;
    double burnedMassFraction;
    
    void computeHeatRelease(FieldManager& fields);
    void computeBurnedMass(FieldManager& fields);
};

} // namespace cfd
