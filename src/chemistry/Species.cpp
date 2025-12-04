#include "chemistry/Species.h"
#include <cmath>
#include <stdexcept>

namespace cfd {

Species::Species() : molecularWeight(1.0), formationEnthalpy(0.0), Tmid(1000.0) {
    nasaLowT.resize(7, 0.0);
    nasaHighT.resize(7, 0.0);
}

Species::Species(const std::string& name_, double molecularWeight_)
    : name(name_), molecularWeight(molecularWeight_), formationEnthalpy(0.0), Tmid(1000.0) {
    nasaLowT.resize(7, 0.0);
    nasaHighT.resize(7, 0.0);
}

void Species::setNASACoeffs(const std::vector<double>& lowT, const std::vector<double>& highT, double Tmid_) {
    if (lowT.size() != 7 || highT.size() != 7) {
        throw std::invalid_argument("NASA coefficients must have 7 elements");
    }
    nasaLowT = lowT;
    nasaHighT = highT;
    Tmid = Tmid_;
}

const std::vector<double>& Species::getCoeffs(double T) const {
    return (T < Tmid) ? nasaLowT : nasaHighT;
}

double Species::evaluateCpOverR(double T) const {
    const auto& a = getCoeffs(T);
    return a[0] + a[1]*T + a[2]*T*T + a[3]*T*T*T + a[4]*T*T*T*T;
}

double Species::evaluateHOverRT(double T) const {
    const auto& a = getCoeffs(T);
    return a[0] + a[1]*T/2.0 + a[2]*T*T/3.0 + a[3]*T*T*T/4.0 + a[4]*T*T*T*T/5.0 + a[5]/T;
}

double Species::evaluateSOverR(double T) const {
    const auto& a = getCoeffs(T);
    return a[0]*std::log(T) + a[1]*T + a[2]*T*T/2.0 + a[3]*T*T*T/3.0 + a[4]*T*T*T*T/4.0 + a[6];
}

double Species::getCp(double T) const {
    double CpOverR = evaluateCpOverR(T);
    double R_specific = R_universal / molecularWeight;  // J/kg/K
    return CpOverR * R_specific;
}

double Species::getH(double T) const {
    double HOverRT = evaluateHOverRT(T);
    double R_specific = R_universal / molecularWeight;  // J/kg/K
    return HOverRT * R_specific * T;
}

double Species::getS(double T) const {
    double SOverR = evaluateSOverR(T);
    double R_specific = R_universal / molecularWeight;  // J/kg/K
    return SOverR * R_specific;
}

} // namespace cfd
