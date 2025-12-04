#pragma once

#include "chemistry/ReactionMechanism.h"
#include <string>
#include <vector>

namespace cfd {

/**
 * @brief Integrates chemical kinetics ODEs
 */
class ChemistryIntegrator {
public:
    ChemistryIntegrator();
    
    // Mechanism loading
    void loadMechanism(const std::string& chemkinFile);
    void setMechanism(const ReactionMechanism& mech);
    
    // Blend composition
    void setBlendComposition(double ethanolFraction);
    
    // Integration
    void integrate(double T, double p, std::vector<double>& Y, double dt);
    
    // Diagnostics
    double getHeatRelease() const { return heatRelease; }
    std::vector<double> getReactionRates() const { return reactionRates; }
    
private:
    ReactionMechanism mechanism;
    double ethanolFraction;
    double heatRelease;
    std::vector<double> reactionRates;
    
    // Integration methods
    void integrateExplicitEuler(double T, double p, std::vector<double>& Y, double dt);
    void integrateImplicit(double T, double p, std::vector<double>& Y, double dt);
};

} // namespace cfd
