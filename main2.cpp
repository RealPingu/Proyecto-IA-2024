#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

// Estructura para representar una ubicación en el espacio 2D
struct Location
{
    int id;
    double x, y;
};

// Estructura para representar a un cliente
struct Customer
{
    Location location;
    double packageWeight;
};

// Estructura para representar un drone
struct Drone
{
    double speed;   // Velocidad constante en m/s
    double maxLoad; // Capacidad máxima de carga en kg
    vector<Customer> assignedCustomers;
};

// Estructura para representar el camión
struct Truck
{
    Location position;
    double speed;
    double launchPenalty;
};

// Estructura para la configuración del problema
struct kMVDRPConfig
{
    Truck truck;
    vector<Drone> drones;
    vector<Customer> customers;
    vector<Location> launchLocations;
};

// Función para calcular la distancia entre dos ubicaciones
double calculateDistance(const Location &a, const Location &b)
{
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

// Calcular el tiempo de viaje dado la distancia y velocidad
double travelTime(double distance, double speed)
{
    return distance / speed;
}

// Asignar clientes a drones y verificar restricciones de viaje y carga máxima
bool assignCustomersToDrones(kMVDRPConfig &config, Location launchPoint, Location recoveryPoint)
{
    bool anyDroneAssigned = false;

    for (size_t droneIdx = 0; droneIdx < config.drones.size(); ++droneIdx)
    {
        auto &drone = config.drones[droneIdx];
        drone.assignedCustomers.clear(); // Limpiar asignaciones previas para esta operación
        double currentLoad = 0.0;

        cout << "\n[Info] Asignando clientes al Drone " << droneIdx + 1 << " desde el punto de lanzamiento ("
             << launchPoint.x << ", " << launchPoint.y << ") hasta el punto de recuperación ("
             << recoveryPoint.x << ", " << recoveryPoint.y << ").\n";

        for (const auto &customer : config.customers)
        {
            double distToCustomer = calculateDistance(launchPoint, customer.location);
            double distToRecovery = calculateDistance(customer.location, recoveryPoint);
            double droneTripTime = travelTime(distToCustomer + distToRecovery, drone.speed);

            // Verificar que el tiempo de operación del dron sea mayor o igual al tiempo del camión
            double truckTripTime = travelTime(calculateDistance(launchPoint, recoveryPoint), config.truck.speed);

            // Verificar capacidad de carga del dron
            if (droneTripTime >= truckTripTime && currentLoad + customer.packageWeight <= drone.maxLoad)
            {
                drone.assignedCustomers.push_back(customer);
                currentLoad += customer.packageWeight;
                anyDroneAssigned = true;

                cout << "  - Drone " << droneIdx + 1 << " asignado al cliente " << customer.location.id
                     << " en la ubicación (" << customer.location.x << ", " << customer.location.y << ") "
                     << "con un paquete de " << customer.packageWeight << " kg. "
                     << "Carga actual del dron: " << currentLoad << " kg.\n";
            }
            else
            {
                cout << "  - Drone " << droneIdx + 1 << " no puede atender al cliente " << customer.location.id
                     << " (ubicación: " << customer.location.x << ", " << customer.location.y
                     << ") por restricciones de carga o tiempo.\n";
            }
        }

        if (drone.assignedCustomers.empty())
        {
            cout << "  [Info] Drone " << droneIdx + 1 << " no tiene asignaciones para esta operación.\n";
        }
    }
    return anyDroneAssigned;
}

// Generar la ruta del camión y lanzar drones desde ubicaciones de lanzamiento
void generateTruckRoute(kMVDRPConfig &config)
{
    Location currentPos = config.truck.position;

    for (const auto &location : config.launchLocations)
    {
        cout << "\n[Movimiento] El camión se mueve al punto de lanzamiento (" << location.x << ", " << location.y << ") para lanzar drones.\n";

        for (const auto &recoveryPoint : config.launchLocations)
        {
            if (assignCustomersToDrones(config, location, recoveryPoint))
            {
                cout << "[Operación] Drones lanzados desde (" << location.x << ", " << location.y
                     << ") y recuperados en (" << recoveryPoint.x << ", " << recoveryPoint.y << ").\n";

                for (size_t droneIdx = 0; droneIdx < config.drones.size(); ++droneIdx)
                {
                    const auto &drone = config.drones[droneIdx];
                    if (!drone.assignedCustomers.empty())
                    {
                        cout << "    [Info] Drone " << droneIdx + 1 << " ruta de entrega:\n";
                        for (const auto &customer : drone.assignedCustomers)
                        {
                            cout << "      - Cliente " << customer.location.id << " en ("
                                 << customer.location.x << ", " << customer.location.y << "), paquete de "
                                 << customer.packageWeight << " kg.\n";
                        }
                    }
                }
            }
        }
        currentPos = location;
    }
    cout << "\n[Fin] El camión regresa al origen.\n";
}

// Ejemplo de ejecución del modelo con la configuración simplificada
int main()
{
    kMVDRPConfig config;
    config.truck = {{0, 0, 0}, 5.0, 10.0};
    config.drones = {{10.0, 5.0}, {10.0, 5.0}}; // Drones con velocidad constante y capacidad máxima de 5 kg
    config.customers = {{{1, 10, 20}, 1.5}, {{2, 15, 25}, 2.0}, {{3, 18, 30}, 1.0}};
    config.launchLocations = {{0, 5, 10}, {1, 20, 25}};

    generateTruckRoute(config);

    return 0;
}
