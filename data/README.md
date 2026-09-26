# Datasets para k-MVDRP

Instancias de prueba (*benchmark*) para evaluar el problema **$k$-Multi-Visit Drone Routing Problem ($k$-MVDRP)**.

## Formato de los Archivos

Cada archivo de dataset utiliza etiquetas de texto y valores numéricos separados por espacios:

```text
# Configuración del camión: velocidad, penalización de lanzamiento, coordenada origen X, coordenada origen Y
TRUCK <speed> <launchPenalty> <origin_x> <origin_y>

# Flota de drones: cantidad, velocidad, carga máxima, energía máx, tasa de consumo, tasa en suspensión
DRONES <count> <speed> <maxLoad> <maxEnergy> <energyDrainRate> <hoverEnergyRate>

# Puntos de lanzamiento y recuperación del camión: cantidad seguida de cada id, x, y
LAUNCH_LOCATIONS <count>
<id> <x> <y>
...

# Lista de clientes: cantidad seguida de cada id, x, y, peso del paquete
CUSTOMERS <count>
<id> <x> <y> <packageWeight>
...
```

## Instancias Disponibles

| Archivo | Clientes | Puntos de Lanzamiento | Drones | Área del Plano |
| :--- | :--- | :--- | :--- | :--- |
| [`instance_small_5.txt`](file:///home/ividal/cpp_projects/Proyecto-IA-2024/data/instance_small_5.txt) | 5 | 2 | 2 | $30 \times 30$ |
| [`instance_medium_15.txt`](file:///home/ividal/cpp_projects/Proyecto-IA-2024/data/instance_medium_15.txt) | 15 | 4 | 3 | $50 \times 50$ |
| [`instance_large_30.txt`](file:///home/ividal/cpp_projects/Proyecto-IA-2024/data/instance_large_30.txt) | 30 | 6 | 4 | $80 \times 80$ |
