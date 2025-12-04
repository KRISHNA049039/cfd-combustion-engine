#pragma once

#include "chemistry/Species.h"
#include <vector>
#include <memory>

namespace cfd {

/**
 * @brief Thermodynamic properties for gas mixtures
 */
class ThermodynamicProperties {
public:
    ThermodynamicProperties();
    
    // Species management
    void addSpecies(const Species& species);
    int getNumSpecies() const { return static_cast<int>(species.size()); }
    const Species& getSpecies(int index) const { return species[index]; }
    
    // Mixture properties
    double getDensity(double T, double p, const std::vector<double>& Y) const;
    double getViscosity(double T, const std::vector<double>& Y) const;
    double getCp(double T, const std::vector<double>& Y) const;
    double getEnthalpy(double T, const std::vector<double>& Y) const;
    double getMolecularWeight(const std::vector<double>& Y) const;
    
    // Individual species properties
    double getSpeciesCp(int speciesIndex, double T) const;
    double getSpeciesH(int speciesIndex, double T) const;
    
    // Equation of state (ideal gas)
    double computePressure(double rho, double T, const std::vector<double>& Y) const;
    double computeTemperature(double rho, double p, const std::vector<double>& Y) const;
    
private:
    std::vector<Species> species;
    
    // Universal gas constant
    static constexpr double R_universal = 8314.46;  // J/kmol/K
    
    // Viscosity mixing rules
    double computeViscosityWilke(double T, const std::vector<double>& Y) const;
    double getSutherlandViscosity(double T, double T0, double mu0, double S) const;
};

} // namespace cfd
