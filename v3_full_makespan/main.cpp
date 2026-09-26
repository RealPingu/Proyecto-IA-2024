#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>

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
    vector<bool> served; // Vector para marcar clientes atendidos
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
bool assignCustomersToDrones(kMVDRPConfig &config, Location launchPoint, Location recoveryPoint, double &maxDroneTime)
{
    bool anyDroneAssigned = false;
    maxDroneTime = 0.0; // Reiniciar el tiempo máximo de los drones para esta operación

    for (size_t droneIdx = 0; droneIdx < config.drones.size(); ++droneIdx)
    {
        auto &drone = config.drones[droneIdx];
        drone.assignedCustomers.clear(); // Limpiar asignaciones previas para esta operación
        double currentLoad = 0.0;

        cout << "\n[Info] Asignando clientes al Drone " << droneIdx + 1 << " desde el punto de lanzamiento ("
             << launchPoint.x << ", " << launchPoint.y << ") hasta el punto de recuperación ("
             << recoveryPoint.x << ", " << recoveryPoint.y << ").\n";

        for (size_t customerIdx = 0; customerIdx < config.customers.size(); ++customerIdx)
        {
            const auto &customer = config.customers[customerIdx];

            // Saltar clientes que ya han sido atendidos
            if (config.served[customerIdx])
            {
                continue;
            }

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
                config.served[customerIdx] = true; // Marcar cliente como atendido
                anyDroneAssigned = true;

                cout << "  - Drone " << droneIdx + 1 << " asignado al cliente " << customer.location.id
                     << " en la ubicación (" << customer.location.x << ", " << customer.location.y << ") "
                     << "con un paquete de " << customer.packageWeight << " kg. "
                     << "Carga actual del dron: " << currentLoad << " kg.\n";

                // Actualizar el tiempo máximo del dron para esta operación
                maxDroneTime = max(maxDroneTime, droneTripTime);
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
double generateTruckRoute(kMVDRPConfig &config)
{
    Location currentPos = config.truck.position;
    double totalTime = 0.0; // Tiempo total de la operación

    for (const auto &location : config.launchLocations)
    {
        // Tiempo que tarda el camión en llegar al punto de lanzamiento
        double truckToLaunchTime = travelTime(calculateDistance(currentPos, location), config.truck.speed);
        totalTime += truckToLaunchTime;

        cout << "\n[Movimiento] El camión se mueve al punto de lanzamiento (" << location.x << ", " << location.y
             << ") para lanzar drones. Tiempo de viaje del camión: " << truckToLaunchTime
             << " unidades de tiempo. Tiempo total acumulado: " << totalTime << "\n";

        for (const auto &recoveryPoint : config.launchLocations)
        {
            double maxDroneTime = 0.0; // Tiempo máximo de operación de drones en esta iteración
            if (assignCustomersToDrones(config, location, recoveryPoint, maxDroneTime))
            {
                cout << "[Operación] Drones lanzados desde (" << location.x << ", " << location.y
                     << ") y recuperados en (" << recoveryPoint.x << ", " << recoveryPoint.y << ").\n";

                // El tiempo de la operación de drones debe ser mayor al del camión en esta operación
                double truckTripTime = travelTime(calculateDistance(location, recoveryPoint), config.truck.speed);
                double operationTime = max(truckTripTime, maxDroneTime) + config.truck.launchPenalty;
                totalTime += operationTime;

                cout << "    - Tiempo de viaje del camión a recuperación: " << truckTripTime << " unidades.\n";
                cout << "    - Tiempo máximo de drones: " << maxDroneTime << " unidades.\n";
                cout << "    - Tiempo de operación actual (incluye penalización de lanzamiento): "
                     << operationTime << " unidades. Tiempo total acumulado: " << totalTime << "\n";

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
        currentPos = location; // Actualizar posición del camión
    }

    // Tiempo de regreso al origen
    double returnToOriginTime = travelTime(calculateDistance(currentPos, config.truck.position), config.truck.speed);
    totalTime += returnToOriginTime;
    cout << "\n[Movimiento] El camión regresa al origen en " << returnToOriginTime
         << " unidades de tiempo. Tiempo total acumulado: " << totalTime << "\n";

    return totalTime;
}

// Cargar instancia desde dataset
bool loadDataset(const string &filepath, kMVDRPConfig &config)
{
    ifstream file(filepath);
    if (!file.is_open())
    {
        cerr << "No se pudo abrir el archivo de dataset: " << filepath << "\n";
        return false;
    }

    string line;
    while (getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        istringstream iss(line);
        string tag;
        iss >> tag;

        if (tag == "TRUCK")
        {
            double speed, penalty, ox, oy;
            iss >> speed >> penalty >> ox >> oy;
            config.truck = {{0, ox, oy}, speed, penalty};
        }
        else if (tag == "DRONES")
        {
            int count;
            double speed, maxLoad;
            iss >> count >> speed >> maxLoad;
            config.drones.clear();
            for (int i = 0; i < count; ++i)
            {
                config.drones.push_back({speed, maxLoad, {}});
            }
        }
        else if (tag == "LAUNCH_LOCATIONS")
        {
            int count;
            iss >> count;
            config.launchLocations.clear();
            for (int i = 0; i < count; ++i)
            {
                getline(file, line);
                while (line.empty() || line[0] == '#')
                    getline(file, line);
                istringstream lss(line);
                int id;
                double x, y;
                lss >> id >> x >> y;
                config.launchLocations.push_back({id, x, y});
            }
        }
        else if (tag == "CUSTOMERS")
        {
            int count;
            iss >> count;
            config.customers.clear();
            for (int i = 0; i < count; ++i)
            {
                getline(file, line);
                while (line.empty() || line[0] == '#')
                    getline(file, line);
                istringstream css(line);
                int id;
                double x, y, weight;
                css >> id >> x >> y >> weight;
                config.customers.push_back({{id, x, y}, weight});
            }
        }
    }
    return true;
}

int main(int argc, char *argv[])
{
    kMVDRPConfig config;

    if (argc > 1)
    {
        cout << "Cargando dataset desde: " << argv[1] << "\n";
        if (!loadDataset(argv[1], config))
        {
            cerr << "Cargando configuración por defecto...\n";
            config.truck = {{0, 0, 0}, 5.0, 10.0};
            config.drones = {{10.0, 5.0, {}}, {10.0, 5.0, {}}};
            config.customers = {{{1, 10, 20}, 1.5}, {{2, 15, 25}, 2.0}, {{3, 18, 30}, 1.0}};
            config.launchLocations = {{0, 5, 10}, {1, 20, 25}};
        }
    }
    else
    {
        cout << "Sin argumento de dataset. Usando configuración por defecto...\n";
        config.truck = {{0, 0, 0}, 5.0, 10.0};
        config.drones = {{10.0, 5.0, {}}, {10.0, 5.0, {}}};
        config.customers = {{{1, 10, 20}, 1.5}, {{2, 15, 25}, 2.0}, {{3, 18, 30}, 1.0}};
        config.launchLocations = {{0, 5, 10}, {1, 20, 25}};
    }

    config.served = vector<bool>(config.customers.size(), false); // Inicializar todos los clientes como no atendidos

    cout << "Problema configurado: " << config.drones.size() << " drones, "
         << config.customers.size() << " clientes, "
         << config.launchLocations.size() << " ubicaciones de lanzamiento.\n";

    double totalTime = generateTruckRoute(config);
    cout << "\n============================================\n";
    cout << "Tiempo total de la operación: " << totalTime << " unidades de tiempo.\n";
    
    // Estadísticas de clientes atendidos
    int servedCount = 0;
    for (bool s : config.served) {
        if (s) servedCount++;
    }
    cout << "Clientes atendidos: " << servedCount << " / " << config.customers.size() << "\n";
    cout << "============================================\n";

    return 0;
}
