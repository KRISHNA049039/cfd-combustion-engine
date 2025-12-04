#pragma once

#include "core/Mesh.h"
#include "core/FieldManager.h"
#include "combustion/SparkIgnition.h"

namespace cfd {

class FlameTracker {
public:
    FlameTracker();
    
    void initializeKernel(const SparkConfig& spark);
    void advectFlame(const FieldManager& fields, double dt);
    double getFlameSpeed(int cellId, const FieldManager& fields) const;
    bool isBurned(int cellId) const;
    
    double getKernelRadius() const { return kernelRadius; }
    
private:
    Vector3D kernelCenter;
    double kernelRadius;
    std::vector<bool> burnedCells;
    
    void updateBurnedRegion(const FieldManager& fields);
};

} // namespace cfd
