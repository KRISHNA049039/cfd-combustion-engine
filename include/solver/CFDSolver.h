#pragma once

#include "core/Mesh.h"
#include "core/FieldManager.h"
#include "solver/FluidDynamics.h"
#include "solver/ThermodynamicProperties.h"
#include "turbulence/TurbulenceModel.h"
#include "combustion/CombustionModel.h"
#include "chemistry/ChemistryIntegrator.h"
#include <memory>

namespace cfd {

struct SimulationConfig {
    double startTime = 0.0;
    double endTime = 0.01;
    double timeStep = 1e-6;
    double outputInterval = 1e-4;
    double checkpointInterval = 1e-3;
    std::string turbulenceModel = "k-epsilon";
    std::string combustionModel = "flamelet";
    int maxIterations = 100;
    double convergenceTolerance = 1e-6;
};

struct InitialConditions {
    double temperature = 300.0;
    double pressure = 101325.0;
    Vector3D velocity = Vector3D(0, 0, 0);
    std::vector<double> massFractions;
};

class CFDSolver {
public:
    CFDSolver();
    ~CFDSolver();
    
    void initialize(const Mesh& mesh, const SimulationConfig& config);
    void setInitialConditions(const InitialConditions& ic);
    bool solve();
    void writeOutput(double time);
    
    double getCurrentTime() const { return currentTime; }
    int getCurrentIteration() const { return currentIteration; }
    
private:
    const Mesh* mesh;
    SimulationConfig config;
    FieldManager fields;
    
    std::unique_ptr<FluidDynamics> fluidSolver;
    std::unique_ptr<TurbulenceModel> turbulenceModel;
    std::unique_ptr<CombustionModel> combustionModel;
    std::unique_ptr<ChemistryIntegrator> chemistryIntegrator;
    std::unique_ptr<ThermodynamicProperties> thermo;
    
    double currentTime;
    int currentIteration;
    
    // Time integration
    void advanceTimeStep(double dt);
    void updateThermodynamics();
    bool checkConvergence();
    
    // Coupling
    void couplePhysics(double dt);
};

} // namespace cfd
