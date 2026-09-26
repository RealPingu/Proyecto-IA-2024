# Multi-visit Drone Routing Problem (k-MVDRP / mFSTSP)

Proyecto de Inteligencia Artificial (2024) — **Universidad Técnica Federico Santa María (UTFSM)**.  
*Basado en la especificación de cátedra y ayudantía (Felipe Guzmán, Septiembre 2024).*

---

## 1. Contexto Académico y Referencia Científica

Este proyecto implementa y modela una variante coordinada de ruteo de vehículos para entregas de última milla (*last-mile delivery*), donde un **camión nodriza** opera en conjunto con una **flota de $k$ drones**.

El marco teórico principal del proyecto corresponde a:

> **Michael Poon, Zhenzhen Luo, Zhiyuan Zhang, Zhixing Liu, Andrew Lim (2021)**  
> *"The Multi-visit Traveling Salesman Problem with Multi-Drones"*  
> **Transportation Research Part C: Emerging Technologies**, Vol. 128, 103168.  
> [Enlace a ScienceDirect](https://doi.org/10.1016/j.trc.2021.103172)

### Referencias Adicionales:
- **Stefan Poikonen & Bruce Golden (2020)**: *"Multi-visit drone routing problem"*, *Computers & Operations Research*, Vol. 113, 104802.
- **Meng et al. (2022)**: *"The multi-visit drone routing problem for pickup and delivery services"*, *Transportation Research Part E*.

---

## 2. Definición del Problema

El objetivo es atender una lista de clientes $C$ entregando sus respectivos paquetes $W$, minimizando el tiempo total de la misión (**makespan**), comenzando y finalizando en un depósito o bodega principal $d$.

```
               [Cliente 1] (paquete w1)
              /           \
             /             \
  (Punto i) ---------------> (Punto j)
   [Camión]                   [Recuperación / Sincronización]
```

### Entidades del Modelo:
- **Camión ($Truck$):** Actúa como bodega móvil y estación de lanzamiento/recuperación. Se desplaza a velocidad $v_{\text{truck}}$ entre los puntos de lanzamiento/recuperación $V$. Incurre en una penalización de tiempo (`launchPenalty`) al preparar el despliegue de drones.
- **Flota de Drones ($k$ Drones):** Unidades aéreas con velocidad $v_{\text{drone}}$ y capacidad máxima de carga útil (`maxLoad`). Pueden entregar uno o más paquetes en una sola salida (*multi-visit*).
- **Clientes ($C$):** Ubicaciones $(x, y)$ en el plano que requieren un paquete con peso $w_c$.
- **Puntos de Lanzamiento y Recuperación ($V$):** Nodos designados en el plano donde el camión puede detenerse a desplegar y recuperar drones.

---

## 3. Restricciones y Consideraciones Específicas del Proyecto

De acuerdo con el documento de especificaciones ([`constraints/MVDRP.pdf`](file:///home/ividal/cpp_projects/Proyecto-IA-2024/constraints/MVDRP.pdf)), el problema cuenta con las siguientes reglas:

1. **Energía Infinita (Sin restricción de batería):**  
   *«Para efectos del proyecto, se consideran drones con energía infinita.»*  
   No se modelan curvas de descarga química de batería ni límites de autonomía en Joules/Wh.
2. **Puntos de Lanzamiento y Recuperación Desacoplados:**  
   Los drones pueden despegar desde un punto $i \in V$ y aterrizar de vuelta en el camión en un punto $j \in V$, donde **no es requisito que $i = j$**.
3. **Velocidad Constante:**  
   La velocidad del dron $v_{\text{drone}}$ se asume constante e independiente de la masa de carga transportada.
4. **Sincronización Estricta por Lotes (*Batch Synchronization*):**  
   **Todos los drones lanzados en una operación deben volver al camión antes de poder realizar nuevos lanzamientos.** No se permiten despliegues asíncronos solapados en diferentes puntos de la ruta del camión.
5. **Condición de Factibilidad Temporal de la Operación:**  
   El tiempo que toma la operación de vuelo del dron debe ser mayor o igual al tiempo de traslado del camión entre el punto de despegue y aterrizaje ($t_{\text{drone}} \ge t_{\text{truck}}$).  
   El tiempo total consumido por la etapa es:
   $$\text{Tiempo Etapa} = \max(t_{\text{truck}}, \max_{m \in k} t_{\text{drone}, m}) + \text{launchPenalty}$$
6. **Finalización de Ruta:**  
   La misión se considera completada únicamente cuando todos los clientes han sido atendidos y tanto el camión como todos los drones han retornado a la bodega central ($d$).

---

## 4. Estructura y Evolución del Código

```text
Proyecto-IA-2024/
├── Makefile                      # Automatización de compilación y pruebas
├── README.md                     # Documentación completa del proyecto
├── .gitignore                    # Reglas de exclusión de Git
├── constraints/                  # Documento de requerimientos de la asignatura
│   └── MVDRP.pdf                 # Presentación de especificación oficial (UTFSM)
├── data/                         # Instancias de prueba estructuradas
│   ├── README.md                 # Especificación del formato de dataset
│   ├── instance_small_5.txt      # Instancia pequeña (5 clientes, 2 puntos, 2 drones)
│   ├── instance_medium_15.txt    # Instancia mediana (15 clientes, 4 puntos, 3 drones)
│   └── instance_large_30.txt     # Instancia grande (30 clientes, 6 puntos, 4 drones)
├── v1_energy_model/              # [Versión 1] Prototipo inicial con modelo de batería
│   └── main.cpp
├── v2_payload_sync/              # [Versión 2] Transición a sincronización y capacidad de carga
│   └── main.cpp
└── v3_full_makespan/             # [Versión 3] Implementación completa con control de estado y makespan
    └── main.cpp
```

### Progresión de las Versiones:

- **[`v1_energy_model/main.cpp`](file:///home/ividal/cpp_projects/Proyecto-IA-2024/v1_energy_model/main.cpp):**  
  Prototipo preliminar exploratorio. Implementó una fórmula de drenaje de batería lineal:
  $$E = \text{energyDrainRate} \times \text{distancia} \times \left(1 + \frac{\text{peso}}{10}\right)$$
  Fue descartado al aplicar la simplificación de energía infinita solicitada en la cátedra.

- **[`v2_payload_sync/main.cpp`](file:///home/ividal/cpp_projects/Proyecto-IA-2024/v2_payload_sync/main.cpp):**  
  Adaptación hacia las restricciones del curso. Remueve el cálculo de batería e introduce la capacidad de carga útil (`maxLoad`), la velocidad constante, y la verificación de tiempos de viaje entre $(L, R)$ para camión y drones.

- **[`v3_full_makespan/main.cpp`](file:///home/ividal/cpp_projects/Proyecto-IA-2024/v3_full_makespan/main.cpp):**  
  Versión final consolidada. Agrega:
  - Vector de clientes atendidos (`vector<bool> served`) para evitar duplicidad de entregas.
  - Cálculo acumulativo del tiempo total de la misión (*makespan*), sumando desplazamientos del camión, tiempos sincronizados de operación aérea, penalizaciones de lanzamiento y regreso a la bodega base.
  - Parser genérico para leer datasets externos desde la línea de comandos.

---

## 5. Instrucciones de Compilación y Ejecución

El proyecto utiliza un [`Makefile`](file:///home/ividal/cpp_projects/Proyecto-IA-2024/Makefile) configurado con `g++` y estándar C++17.

### Compilar todos los ejecutables
```bash
make
```
Los binarios se generarán en la carpeta `bin/`:
- `bin/v1_energy`
- `bin/v2_payload_sync`
- `bin/v3_full_makespan`

### Ejecutar con los Datasets incluidos
```bash
# Ejecutar Versión 3 con instancia pequeña (5 clientes)
make run

# Ejecutar Versión 3 con instancia mediana (15 clientes)
make run-medium

# Ejecutar Versión 3 con instancia grande (30 clientes)
make run-large

# Ejecutar versiones anteriores
make run-v1
make run-v2
```

### Ejecutar manualmente con un dataset personalizado
```bash
./bin/v3_full_makespan ruta/a/tu_archivo.txt
```

### Limpieza de binarios
```bash
make clean
```
