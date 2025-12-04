#pragma once

namespace cfd {

class LaminarFlameSpeed {
public:
    LaminarFlameSpeed();
    
    // Correlations
    double computeGulder(double T, double p, double phi, double ethanolFraction) const;
    double computeMetghalchiKeck(double T, double p, double phi) const;
    
    // Turbulent flame speed
    double computeTurbulentFlameSpeed(double Sl, double uPrime, double k) const;
    
private:
    // Reference conditions
    double T0 = 298.0;  // K
    double p0 = 101325.0;  // Pa
};

} // namespace cfd
