#pragma once

#include <string>
#include <vector>

namespace cfd {

/**
 * @brief Chemical species with thermodynamic properties
 * Uses NASA polynomial format for temperature-dependent properties
 */
class Species {
public:
    Species();
    Species(const std::string& name_, double molecularWeight_);
    
    // Accessors
    std::string getName() const { return name; }
    double getMolecularWeight() const { return molecularWeight; }
    
    // NASA polynomial coefficients
    void setNASACoeffs(const std::vector<double>& lowT, const std::vector<double>& highT, double Tmid);
    
    // Thermodynamic properties (temperature-dependent)
    double getCp(double T) const;  // Specific heat at constant pressure [J/kg/K]
    double getH(double T) const;   // Enthalpy [J/kg]
    double getS(double T) const;   // Entropy [J/kg/K]
    
    // Formation properties
    void setFormationEnthalpy(double hf) { formationEnthalpy = hf; }
    double getFormationEnthalpy() const { return formationEnthalpy; }
    
private:
    std::string name;
    double molecularWeight;  // kg/kmol
    double formationEnthalpy;  // J/kmol
    
    // NASA polynomial coefficients
    // Cp/R = a1 + a2*T + a3*T^2 + a4*T^3 + a5*T^4
    // H/RT = a1 + a2*T/2 + a3*T^2/3 + a4*T^3/4 + a5*T^4/5 + a6/T
    // S/R = a1*ln(T) + a2*T + a3*T^2/2 + a4*T^3/3 + a5*T^4/4 + a7
    std::vector<double> nasaLowT;   // 7 coefficients for low temperature range
    std::vector<double> nasaHighT;  // 7 coefficients for high temperature range
    double Tmid;  // Transition temperature between ranges
    
    // Universal gas constant
    static constexpr double R_universal = 8314.46;  // J/kmol/K
    
    // Helper methods
    const std::vector<double>& getCoeffs(double T) const;
    double evaluateCpOverR(double T) const;
    double evaluateHOverRT(double T) const;
    double evaluateSOverR(double T) const;
};

} // namespace cfd
