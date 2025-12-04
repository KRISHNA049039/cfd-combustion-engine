#pragma once

#include "core/Mesh.h"
#include "core/FieldManager.h"

namespace cfd {

/**
 * @brief Abstract base class for turbulence models
 */
class TurbulenceModel {
public:
    virtual ~TurbulenceModel() = default;
    
    // Pure virtual methods
    virtual void solve(FieldManager& fields, double dt) = 0;
    virtual double getTurbulentViscosity(int cellId) const = 0;
    virtual double getTurbulentKineticEnergy(int cellId) const = 0;
    
    // Initialization
    virtual void initialize(const Mesh& mesh, FieldManager& fields) = 0;
    
protected:
    const Mesh* mesh;
    
    TurbulenceModel() : mesh(nullptr) {}
};

} // namespace cfd
