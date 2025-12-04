#pragma once

#include "turbulence/TurbulenceModel.h"

namespace cfd {

/**
 * @brief Standard k-epsilon turbulence model
 */
class KEpsilonModel : public TurbulenceModel {
public:
    KEpsilonModel();
    ~KEpsilonModel() override = default;
    
    void initialize(const Mesh& mesh, FieldManager& fields) override;
    void solve(FieldManager& fields, double dt) override;
    double getTurbulentViscosity(int cellId) const override;
    double getTurbulentKineticEnergy(int cellId) const override;
    
    // Wall functions
    void applyWallFunctions(const Mesh& mesh, FieldManager& fields);
    
private:
    // Model constants
    double Cmu = 0.09;
    double C1 = 1.44;
    double C2 = 1.92;
    double sigmaK = 1.0;
    double sigmaEps = 1.3;
    
    // Cached values
    std::vector<double> turbulentViscosity;
    
    // Helper methods
    void solveKEquation(FieldManager& fields, double dt);
    void solveEpsilonEquation(FieldManager& fields, double dt);
    void updateTurbulentViscosity(FieldManager& fields);
};

} // namespace cfd
