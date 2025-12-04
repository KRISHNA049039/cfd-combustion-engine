#include "chemistry/ReactionMechanism.h"
#include <cmath>
#include <algorithm>

namespace cfd {

ReactionMechanism::ReactionMechanism() {
}

void ReactionMechanism::addSpecies(const Species& spec) {
    int index = static_cast<int>(species.size());
    species.push_back(spec);
    speciesIndex[spec.getName()] = index;
}

std::string ReactionMechanism::getSpeciesName(int index) const {
    if (index >= 0 && index < static_cast<int>(species.size())) {
        return species[index].getName();
    }
    return "";
}

int ReactionMechanism::getSpeciesIndex(const std::string& name) const {
    auto it = speciesIndex.find(name);
    if (it != speciesIndex.end()) {
        return it->second;
    }
    return -1;
}

void ReactionMechanism::addReaction(const Reaction& reaction) {
    reactions.push_back(reaction);
}

void ReactionMechanism::computeRates(double T, double p, const std::vector<double>& Y,
                                    std::vector<double>& omega) {
    // Initialize production rates
    omega.resize(species.size(), 0.0);
    
    // Compute concentrations [kmol/m^3]
    std::vector<double> C;
    computeConcentrations(T, p, Y, C);
    
    // Compute rate for each reaction
    for (size_t i = 0; i < reactions.size(); ++i) {
        const Reaction& rxn = reactions[i];
        
        // Forward rate constant
        double kf = computeForwardRate(i, T);
        
        // Forward rate of progress
        double qf = kf;
        for (size_t j = 0; j < rxn.reactants.size(); ++j) {
            int specIdx = rxn.reactants[j];
            double nu = rxn.stoichReactants[j];
            qf *= std::pow(C[specIdx], nu);
        }
        
        // Reverse rate of progress
        double qr = 0.0;
        if (rxn.reversible) {
            double kr = computeReverseRate(i, T, p, C);
            qr = kr;
            for (size_t j = 0; j < rxn.products.size(); ++j) {
                int specIdx = rxn.products[j];
                double nu = rxn.stoichProducts[j];
                qr *= std::pow(C[specIdx], nu);
            }
        }
        
        // Net rate of progress
        double q = qf - qr;
        
        // Update species production rates
        for (size_t j = 0; j < rxn.reactants.size(); ++j) {
            int specIdx = rxn.reactants[j];
            omega[specIdx] -= rxn.stoichReactants[j] * q;
        }
        for (size_t j = 0; j < rxn.products.size(); ++j) {
            int specIdx = rxn.products[j];
            omega[specIdx] += rxn.stoichProducts[j] * q;
        }
    }
    
    // Convert from [kmol/m^3/s] to [kg/m^3/s]
    for (size_t i = 0; i < omega.size(); ++i) {
        omega[i] *= species[i].getMolecularWeight();
    }
}

double ReactionMechanism::computeForwardRate(int reactionIndex, double T) const {
    const Reaction& rxn = reactions[reactionIndex];
    // k = A * T^beta * exp(-Ea/RT)
    return rxn.A * std::pow(T, rxn.beta) * std::exp(-rxn.Ea / (R_universal * T));
}

double ReactionMechanism::computeReverseRate(int reactionIndex, double T, double p,
                                            const std::vector<double>& concentrations) const {
    // kr = kf / Kc
    double kf = computeForwardRate(reactionIndex, T);
    double Kc = computeEquilibriumConstant(reactionIndex, T);
    return (Kc > 1e-30) ? (kf / Kc) : 0.0;
}

void ReactionMechanism::computeConcentrations(double T, double p, const std::vector<double>& Y,
                                             std::vector<double>& C) const {
    C.resize(species.size());
    
    // C_i = (rho * Y_i) / MW_i
    // rho = p * MW_mix / (R * T)
    
    // Compute mixture molecular weight
    double invMW = 0.0;
    for (size_t i = 0; i < species.size(); ++i) {
        invMW += Y[i] / species[i].getMolecularWeight();
    }
    double MW_mix = (invMW > 1e-10) ? (1.0 / invMW) : 28.97;
    
    // Compute density
    double R_specific = R_universal / MW_mix;
    double rho = p / (R_specific * T);
    
    // Compute concentrations
    for (size_t i = 0; i < species.size(); ++i) {
        C[i] = (rho * Y[i]) / species[i].getMolecularWeight();
    }
}

double ReactionMechanism::computeEquilibriumConstant(int reactionIndex, double T) const {
    // Simplified: Kc = exp(deltaS/R - deltaH/RT)
    // In production, would use proper thermodynamic data
    return 1.0;  // Placeholder
}

} // namespace cfd
