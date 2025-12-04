#include "combustion/LaminarFlameSpeed.h"
#include <cmath>

namespace cfd {

LaminarFlameSpeed::LaminarFlameSpeed() {
}

double LaminarFlameSpeed::computeGulder(double T, double p, double phi, double ethanolFraction) const {
    // Gulder correlation for laminar flame speed
    // Sl = Sl0 * (T/T0)^alpha * (p/p0)^beta
    
    double Sl0 = 0.4;  // m/s (reference flame speed)
    double alpha = 1.75;
    double beta = -0.5;
    
    // Adjust for ethanol content
    Sl0 *= (1.0 + 0.1 * ethanolFraction);
    
    // Adjust for equivalence ratio
    double phiFactor = 1.0 - 0.5 * std::pow(phi - 1.0, 2.0);
    
    return Sl0 * std::pow(T / T0, alpha) * std::pow(p / p0, beta) * phiFactor;
}

double LaminarFlameSpeed::computeMetghalchiKeck(double T, double p, double phi) const {
    // Metghalchi-Keck correlation
    double Sl0 = 0.4;
    double alpha = 2.0;
    double beta = -0.5;
    
    return Sl0 * std::pow(T / T0, alpha) * std::pow(p / p0, beta);
}

double LaminarFlameSpeed::computeTurbulentFlameSpeed(double Sl, double uPrime, double k) const {
    // Turbulent flame speed: St = Sl + C * u'
    double C = 1.0;  // Constant
    double turbIntensity = std::sqrt(2.0 * k / 3.0);  // From k
    
    return Sl + C * turbIntensity;
}

} // namespace cfd
