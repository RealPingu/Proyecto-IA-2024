#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>
#include <sstream>

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
        return energyDrainRate * distance * (1 + weight / 10.0); // Simplificado
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
bool isEnergyFeasible(const Drone& drone, const std::vector<Customer>& customers) {
    if (customers.empty()) return true;

    double totalEnergy = 0.0;
    double weight = 0.0;
    Location currentPosition = customers[0].location;

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
    std::cout << "\n=== Asignación de Clientes a Drones (Modelo de Energía) ===\n";
    for (auto& customer : config.customers) {
        bool assigned = false;
        for (size_t i = 0; i < config.drones.size(); ++i) {
            auto& drone = config.drones[i];
            // Intentar asignar al primer drone factible
            drone.assignedCustomers.push_back(customer);
            if (isEnergyFeasible(drone, drone.assignedCustomers)) {
                std::cout << "Cliente " << customer.location.id << " (peso: " << customer.packageWeight
                          << " kg) asignado a Drone " << i + 1 << ".\n";
                assigned = true;
                break;
            } else {
                drone.assignedCustomers.pop_back(); // Deshacer si no es factible
            }
        }
        if (!assigned) {
            std::cout << "Cliente " << customer.location.id << " no pudo ser asignado (límite de energía alcanzado).\n";
        }
    }
}

// Generar la ruta del camión con paradas de lanzamiento de drones
void generateTruckRoute(kMVDRPConfig& config) {
    std::cout << "\n=== Ruta del Camión ===\n";
    Location currentPos = config.truck.position;
    for (const auto& location : config.launchLocations) {
        // Lanzar drones desde la ubicación de lanzamiento
        std::cout << "Camión se mueve a (" << location.x << ", " << location.y << ") y lanza drones.\n";
        currentPos = location;
    }
    std::cout << "Camión regresa al origen (" << config.truck.position.x << ", " << config.truck.position.y << ").\n";
}

// Cargar instancia desde dataset
bool loadDataset(const std::string& filepath, kMVDRPConfig& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo de dataset: " << filepath << "\n";
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string tag;
        iss >> tag;

        if (tag == "TRUCK") {
            double speed, penalty, ox, oy;
            iss >> speed >> penalty >> ox >> oy;
            config.truck = {{0, ox, oy}, speed, penalty};
        } else if (tag == "DRONES") {
            int count;
            double speed, maxLoad, maxEnergy, drainRate, hoverRate;
            iss >> count >> speed >> maxLoad >> maxEnergy >> drainRate >> hoverRate;
            config.drones.clear();
            for (int i = 0; i < count; ++i) {
                config.drones.push_back({maxEnergy, drainRate, hoverRate, speed, {}});
            }
        } else if (tag == "LAUNCH_LOCATIONS") {
            int count;
            iss >> count;
            config.launchLocations.clear();
            for (int i = 0; i < count; ++i) {
                std::getline(file, line);
                while (line.empty() || line[0] == '#') std::getline(file, line);
                std::istringstream lss(line);
                int id;
                double x, y;
                lss >> id >> x >> y;
                config.launchLocations.push_back({id, x, y});
            }
        } else if (tag == "CUSTOMERS") {
            int count;
            iss >> count;
            config.customers.clear();
            for (int i = 0; i < count; ++i) {
                std::getline(file, line);
                while (line.empty() || line[0] == '#') std::getline(file, line);
                std::istringstream css(line);
                int id;
                double x, y, weight;
                css >> id >> x >> y >> weight;
                config.customers.push_back({{id, x, y}, weight});
            }
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    kMVDRPConfig config;

    if (argc > 1) {
        std::cout << "Cargando dataset desde: " << argv[1] << "\n";
        if (!loadDataset(argv[1], config)) {
            std::cerr << "Cargando configuración por defecto...\n";
            config.truck = {{0, 0, 0}, 5.0, 10.0};
            config.drones = {{100.0, 1.5, 0.5, 10.0, {}}, {100.0, 1.5, 0.5, 10.0, {}}};
            config.customers = {{{1, 10, 20}, 1.5}, {{2, 15, 25}, 2.0}};
            config.launchLocations = {{0, 5, 10}, {1, 20, 25}};
        }
    } else {
        std::cout << "Sin argumento de dataset. Usando configuración por defecto...\n";
        config.truck = {{0, 0, 0}, 5.0, 10.0};
        config.drones = {{100.0, 1.5, 0.5, 10.0, {}}, {100.0, 1.5, 0.5, 10.0, {}}};
        config.customers = {{{1, 10, 20}, 1.5}, {{2, 15, 25}, 2.0}};
        config.launchLocations = {{0, 5, 10}, {1, 20, 25}};
    }

    std::cout << "Problema configurado: " << config.drones.size() << " drones, "
              << config.customers.size() << " clientes, "
              << config.launchLocations.size() << " ubicaciones de lanzamiento.\n";

    assignCustomersToDrones(config);
    generateTruckRoute(config);

    return 0;
}
