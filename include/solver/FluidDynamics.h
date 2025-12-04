#pragma once

#include "core/Mesh.h"
#include "core/FieldManager.h"
#include "solver/ThermodynamicProperties.h"

namespace cfd {

/**
 * @brief Fluid dynamics solver for compressible Navier-Stokes equations
 */
class FluidDynamics {
public:
    FluidDynamics();
    
    // Initialization
    void initialize(const Mesh& mesh, FieldManager& fields);
    void setThermodynamicProperties(ThermodynamicProperties* thermo);
    
    // Solver steps
    void computeMomentum(FieldManager& fields, double dt);
    void solvePressureCorrection(FieldManager& fields);
    void updateVelocity(FieldManager& fields);
    void solveEnergy(FieldManager& fields, double dt);
    
    // Diagnostics
    double getMaxCourantNumber() const { return maxCourantNumber; }
    
private:
    const Mesh* mesh;
    ThermodynamicProperties* thermo;
    double maxCourantNumber;
    
    // Convection schemes
    double computeConvectiveFlux(int faceId, const Field& phi, const Field& velocity);
    double computeDiffusiveFlux(int faceId, const Field& phi);
    
    // SIMPLE algorithm helpers
    void assembleM omentumMatrix();
    void assemblePressureMatrix();
    void correctVelocity();
};

} // namespace cfd
