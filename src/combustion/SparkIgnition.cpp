#include "combustion/SparkIgnition.h"
#include "core/Mesh.h"

namespace cfd {

SparkIgnition::SparkIgnition() : applied(false) {
}

void SparkIgnition::initialize(const SparkConfig& config_) {
    config = config_;
    applied = false;
}

void SparkIgnition::apply(FieldManager& fields, double time, const Mesh& mesh) {
    if (applied || time < config.ignitionTime) {
        return;
    }
    
    depositEnergy(fields, mesh);
    applied = true;
}

bool SparkIgnition::isActive(double time) const {
    return (time >= config.ignitionTime) && !applied;
}

void SparkIgnition::depositEnergy(FieldManager& fields, const Mesh& mesh) {
    Field& temperature = fields.getField("temperature");
    
    // Find cells within spark kernel radius
    for (int i = 0; i < mesh.getNumCells(); ++i) {
        const Cell& cell = mesh.getCell(i);
        double dist = (cell.centroid - config.location).magnitude();
        
        if (dist < config.kernelRadius) {
            // Deposit energy as temperature rise
            temperature(i) += 500.0;  // K (simplified)
        }
    }
}

} // namespace cfd
