#pragma once

#include "chemistry/Species.h"
#include <vector>
#include <string>
#include <map>

namespace cfd {

/**
 * @brief Chemical reaction representation
 */
struct Reaction {
    std::vector<int> reactants;
    std::vector<int> products;
    std::vector<double> stoichReactants;
    std::vector<double> stoichProducts;
    
    // Arrhenius parameters: k = A * T^beta * exp(-Ea/RT)
    double A;      // Pre-exponential factor
    double beta;   // Temperature exponent
    double Ea;     // Activation energy [J/kmol]
    
    bool reversible;
    
    Reaction() : A(0.0), beta(0.0), Ea(0.0), reversible(true) {}
};

/**
 * @brief Chemical reaction mechanism
 */
class ReactionMechanism {
public:
    ReactionMechanism();
    
    // Species management
    void addSpecies(const Species& species);
    int getNumSpecies() const { return static_cast<int>(species.size()); }
    std::string getSpeciesName(int index) const;
    int getSpeciesIndex(const std::string& name) const;
    const Species& getSpecies(int index) const { return species[index]; }
    
    // Reaction management
    void addReaction(const Reaction& reaction);
    int getNumReactions() const { return static_cast<int>(reactions.size()); }
    const Reaction& getReaction(int index) const { return reactions[index]; }
    
    // Rate computation
    void computeRates(double T, double p, const std::vector<double>& Y,
                     std::vector<double>& omega);
    
    double computeForwardRate(int reactionIndex, double T) const;
    double computeReverseRate(int reactionIndex, double T, double p,
                             const std::vector<double>& concentrations) const;
    
private:
    std::vector<Species> species;
    std::vector<Reaction> reactions;
    std::map<std::string, int> speciesIndex;
    
    static constexpr double R_universal = 8314.46;  // J/kmol/K
    
    // Helper methods
    void computeConcentrations(double T, double p, const std::vector<double>& Y,
                              std::vector<double>& C) const;
    double computeEquilibriumConstant(int reactionIndex, double T) const;
};

} // namespace cfd
