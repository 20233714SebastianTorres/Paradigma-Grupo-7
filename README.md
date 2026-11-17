# Planificador de Servicios de Mudanza en Lima Metropolitana

**Proyecto - Paradigmas de Programación (Grupo 7)**  
Sistema logístico y de ruteo para una empresa de mudanzas en Lima Metropolitana implementado bajo tres paradigmas de programación:
- **Imperativo:** C++
- **Funcional:** Haskell
- **Lógico:** Prolog

La persistencia y el intercambio de información entre los tres lenguajes se realiza de manera común y desacoplada mediante archivos CSV ubicados en `datos/`:
- `puntos.csv`: almacena depósitos y domicilios de clientes (código, nombre, tipo, zona).
- `conexiones.csv`: tramos viales entre puntos (origen, destino, distancia km, tiempo min, estado habilitada/cerrada).
- `unidades.csv`: flota de camiones (código, tipo, capacidad m³, costo por km, estado disponible/ocupada).
- `pedidos.csv`: solicitudes de mudanza (código, origen, destino, carga m³, prioridad, estado, camión asignado, fecha, franja horaria).

---

## 1. Módulo en C++ (Paradigma Imperativo)

El módulo en C++ registra información con validaciones, gestiona estados viales y de pedidos, realiza consultas analíticas del sistema y cotizaciones comparativas.

### Compilación

Ejecutar desde la raíz del proyecto:

#### Con MSVC (Visual Studio Developer PowerShell / CMD):
```powershell
cl.exe /std:c++17 /EHsc /I cpp\include cpp\src\main.cpp cpp\src\menu.cpp cpp\src\persistencia.cpp cpp\src\integrante2.cpp cpp\src\integrante3.cpp /Fe:mudanzas.exe
```

#### Con GCC / MinGW (Linux / macOS / Git Bash):
```bash
g++ -std=c++17 -I cpp/include cpp/src/main.cpp cpp/src/menu.cpp cpp/src/persistencia.cpp cpp/src/integrante2.cpp cpp/src/integrante3.cpp -o mudanzas.exe
```

### Ejecución
```powershell
.\mudanzas.exe
```

### Opciones del Menú Principal:
- **1 al 4**: Registro con validaciones de puntos, conexiones, camiones y mudanzas.
- **5 al 7**: Gestión de estados (cerrar/habilitar vía, asignar mudanza a camión, marcar mudanza entregada).
- **8 al 11**: Consultas básicas (listar puntos, conexiones de un punto, mudanzas por estado, mudanzas por camión).
- **12**: Reporte general del sistema (conteo de infraestructura, flota, estado de pedidos y cargas en tránsito).
- **13**: Agendar servicio con fecha y franja horaria (Funcionalidad Adicional - Integrante 2).
- **14**: Cotización comparativa y recomendación automática de camión económico (Funcionalidad Adicional - Integrante 3).
- **15 y 16**: Recargar y guardar cambios en los archivos CSV.
- **17**: Ver resumen de registros en memoria.
- **0**: Salir del programa.

---

## 2. Módulo en Haskell (Paradigma Funcional)

El módulo en Haskell lee los archivos CSV generados y realiza cálculos puros sobre grafos, análisis de conectividad y optimización de costos.

### Requisitos
Tener instalado el entorno de Haskell (`ghc` / `runghc` / `ghci`).

### Ejecución de módulos:

#### A. Módulo del Integrante 1 (Consolidación de mudanzas):
```bash
runghc -ihaskell haskell/Main.hs
```
Calcula la agrupación de pedidos pendientes de la misma zona (`ahorroConsolidacion ["m002", "m006"] "c02" datos`) comparando el costo individual frente al trayecto consolidado y mostrando el ahorro obtenido.

#### B. Módulo del Integrante 3 (Conectividad y Cotización):
Abrir la consola interactiva:
```bash
ghci -ihaskell haskell/Integrante3.hs
```
Dentro de GHCi, ejecutar las consultas:
```haskell
-- 1. Cargar la base de datos CSV
import LecturaCSV
Right dt <- leerDatos "datos"

-- 2. Consultar vecinos conectados por vías habilitadas (4.2.3)
vecinos "p01" (conexiones dt)

-- 3. Generar todas las rutas posibles sin ciclos entre dos puntos (4.2.4)
rutasPosibles "p01" "p05" (conexiones dt)

-- 4. Calcular el costo total de una ruta para un camión (4.2.5)
let (cam02:_) = filter (\c -> codigoCamion c == "c02") (camiones dt)
costoRuta ["p01", "p02", "p04", "p05"] cam02 (conexiones dt)

-- 5. Cotización comparativa y recomendación de camión (Funcionalidad Adicional)
cotizarServicio "m001" dt
```

---

## 3. Módulo en Prolog (Paradigma Lógico)

El módulo en Prolog modela la base de conocimiento y resuelve consultas lógicas mediante reglas, unificación y backtracking.

### Requisitos
Tener instalado SWI-Prolog (`swipl`).

### Ejecución de módulos:

#### A. Conectividad, alcanzabilidad y agenda de servicios (`prolog/integrante2.pl`):
Cargar el archivo en SWI-Prolog:
```bash
swipl -s prolog/consolidacion.pl -g cargar_datos -l prolog/integrante2.pl
```
O desde la consola de SWI-Prolog:
```prolog
?- [prolog/consolidacion].
?- cargar_datos.
?- [prolog/integrante2].
```

**Consultas de ejemplo:**
```prolog
% 4.3.2 Conexión directa habilitada entre dos puntos:
?- conectado(p01, p02, Distancia).

% 4.3.3 Alcanzabilidad directa o indirecta entre dos puntos:
?- alcanzable(p01, p06).

% Funcionalidad Adicional: Comprobar si un camión está libre en una fecha y franja horaria:
?- camion_libre(c02, '2026-11-20', tarde).
?- camion_libre(c01, '2026-11-20', manana).
```

#### B. Consolidación de mudanzas pequeñas (`prolog/consolidacion.pl`):
```bash
swipl -s prolog/consolidacion.pl
```

**Consultas de ejemplo:**
```prolog
% Cargar la base de conocimiento desde los CSVs:
?- cargar_datos.

% Encontrar combinaciones de pedidos pendientes consolidables en un camión disponible:
?- consolidable(c02, ListaPedidos).

% Determinar la mejor consolidación que maximiza el ahorro monetario:
?- mejor_consolidacion(c02, ListaPedidos, Ahorro).

% Calcular el ahorro obtenido para un grupo específico de pedidos:
?- ahorro_grupo(c02, [m002, m006], Ahorro).
```

---

## Estructura del Proyecto

```text
├── cpp/
│   ├── include/
│   │   ├── menu.hpp
│   │   ├── modelos.hpp
│   │   ├── persistencia.hpp
│   │   ├── integrante2.hpp
│   │   └── integrante3.hpp
│   └── src/
│       ├── main.cpp
│       ├── menu.cpp
│       ├── persistencia.cpp
│       ├── integrante2.cpp
│       └── integrante3.cpp
├── datos/
│   ├── conexiones.csv
│   ├── pedidos.csv
│   ├── puntos.csv
│   └── unidades.csv
├── haskell/
│   ├── Consolidacion.hs
│   ├── Integrante3.hs
│   ├── LecturaCSV.hs
│   ├── Main.hs
│   └── Tipos.hs
├── prolog/
│   ├── consolidacion.pl
│   └── integrante2.pl
├── README.md
├── README_Integrante1.md
└── README_Integrante3.md
```
