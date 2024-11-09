#include <iostream>
#include <vector>
#include <cmath>

// Estructura para representar una ubicación en el espacio 2D
struct Location {
    int id;
    double x, y;
};

// Estructura para representar a un cliente
struct Customer {
    Location location;
    double packageWeight;
};

// Estructura para representar un drone
struct Drone {
    double maxEnergy;
    double energyDrainRate;
    double hoverEnergyRate;
    double speed; // en metros por segundo
    std::vector<Customer> assignedCustomers;

    // Calcular la energía usada en función del peso y la distancia
    double calculateEnergyUse(double weight, double distance) const {
        return energyDrainRate * distance * (1 + weight / 10); // Simplificado
    }
};

// Estructura para representar el camión
struct Truck {
    Location position;
    double speed;
    double launchPenalty;
};

// Estructura para la configuración del problema
struct kMVDRPConfig {
    Truck truck;
    std::vector<Drone> drones;
    std::vector<Customer> customers;
    std::vector<Location> launchLocations;
};

double calculateDistance(const Location& a, const Location& b) {
    return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}

// Verificar si un vuelo de drone es energéticamente factible
bool isEnergyFeasible(const Drone&  drone, const std::vector<Customer>& customers) {
    double totalEnergy = 0;
    double weight = 0;
    Location currentPosition = drone.assignedCustomers[0].location;

    for (const auto& customer : customers) {
        double distance = calculateDistance(currentPosition, customer.location);
        totalEnergy += drone.calculateEnergyUse(weight, distance);
        if (totalEnergy > drone.maxEnergy) return false;
        weight += customer.packageWeight;
        currentPosition = customer.location;
    }
    return true;
}

// Asignar clientes a los drones
void assignCustomersToDrones(kMVDRPConfig& config) {
    for (auto& customer : config.customers) {
        for (auto& drone : config.drones) {
            // Intentar asignar al primer drone factible
            drone.assignedCustomers.push_back(customer);
            if (isEnergyFeasible(drone, drone.assignedCustomers)) {
                break;
            } else {
                drone.assignedCustomers.pop_back(); // Deshacer si no es factible
            }
        }
    }
}

// Generar la ruta del camión con paradas de lanzamiento de drones
void generateTruckRoute(kMVDRPConfig& config) {
    Location currentPos = config.truck.position;
    for (const auto& location : config.launchLocations) {
        // Lanzar drones desde la ubicación de lanzamiento
        std::cout << "Truck moves to (" << location.x << ", " << location.y << ") and launches drones.\n";
        currentPos = location;
    }
    std::cout << "Truck returns to origin.\n";
}

int main() {
    // Configuración del problema
    kMVDRPConfig config;
    // Definir el camión y drones
    config.truck = { {0, 0, 0}, 5.0, 10.0 };
    config.drones = { {100.0, 1.5, 0.5, 10.0}, {100.0, 1.5, 0.5, 10.0} };
    // Agregar clientes y ubicaciones de lanzamiento (ejemplo)
    config.customers = { {{1, 10, 20}, 1.5}, {{2, 15, 25}, 2.0} };
    config.launchLocations = { {0, 5, 10}, {1, 20, 25} };

    assignCustomersToDrones(config);
    generateTruckRoute(config);

    return 0;
}
