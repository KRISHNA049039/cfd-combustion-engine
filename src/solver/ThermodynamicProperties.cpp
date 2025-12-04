#include "solver/ThermodynamicProperties.h"
#include <cmath>
#include <algorithm>

namespace cfd {

ThermodynamicProperties::ThermodynamicProperties() {
}

void ThermodynamicProperties::addSpecies(const Species& spec) {
    species.push_back(spec);
}

double ThermodynamicProperties::getMolecularWeight(const std::vector<double>& Y) const {
    // Mixture molecular weight from mass fractions
    // 1/MW_mix = sum(Y_i / MW_i)
    double invMW = 0.0;
    for (size_t i = 0; i < species.size() && i < Y.size(); ++i) {
        invMW += Y[i] / species[i].getMolecularWeight();
    }
    return (invMW > 1e-10) ? (1.0 / invMW) : 28.97;  // Default to air MW
}

double ThermodynamicProperties::getDensity(double T, double p, const std::vector<double>& Y) const {
    // Ideal gas law: rho = p * MW / (R * T)
    double MW = getMolecularWeight(Y);
    double R_specific = R_universal / MW;  // J/kg/K
    return p / (R_specific * T);
}

double ThermodynamicProperties::getCp(double T, const std::vector<double>& Y) const {
    // Mass-weighted average
    double Cp_mix = 0.0;
    for (size_t i = 0; i < species.size() && i < Y.size(); ++i) {
        Cp_mix += Y[i] * species[i].getCp(T);
    }
    return Cp_mix;
}

double ThermodynamicProperties::getEnthalpy(double T, const std::vector<double>& Y) const {
    // Mass-weighted average
    double H_mix = 0.0;
    for (size_t i = 0; i < species.size() && i < Y.size(); ++i) {
        H_mix += Y[i] * species[i].getH(T);
    }
    return H_mix;
}

double ThermodynamicProperties::getViscosity(double T, const std::vector<double>& Y) const {
    return computeViscosityWilke(T, Y);
}

double ThermodynamicProperties::computeViscosityWilke(double T, const std::vector<double>& Y) const {
    // Simplified Wilke's mixing rule
    // For now, use Sutherland's law for air as approximation
    double T0 = 273.15;  // K
    double mu0 = 1.716e-5;  // Pa·s
    double S = 110.4;  // K
    
    return getSutherlandViscosity(T, T0, mu0, S);
}

double ThermodynamicProperties::getSutherlandViscosity(double T, double T0, double mu0, double S) const {
    // Sutherland's law: mu = mu0 * (T/T0)^1.5 * (T0 + S) / (T + S)
    return mu0 * std::pow(T / T0, 1.5) * (T0 + S) / (T + S);
}

double ThermodynamicProperties::computePressure(double rho, double T, const std::vector<double>& Y) const {
    // Ideal gas: p = rho * R * T / MW
    double MW = getMolecularWeight(Y);
    double R_specific = R_universal / MW;
    return rho * R_specific * T;
}

double ThermodynamicProperties::computeTemperature(double rho, double p, const std::vector<double>& Y) const {
    // Ideal gas: T = p * MW / (rho * R)
    double MW = getMolecularWeight(Y);
    double R_specific = R_universal / MW;
    return p / (rho * R_specific);
}

double ThermodynamicProperties::getSpeciesCp(int speciesIndex, double T) const {
    if (speciesIndex >= 0 && speciesIndex < static_cast<int>(species.size())) {
        return species[speciesIndex].getCp(T);
    }
    return 1000.0;  // Default value
}

double ThermodynamicProperties::getSpeciesH(int speciesIndex, double T) const {
    if (speciesIndex >= 0 && speciesIndex < static_cast<int>(species.size())) {
        return species[speciesIndex].getH(T);
    }
    return 0.0;
}

} // namespace cfd
