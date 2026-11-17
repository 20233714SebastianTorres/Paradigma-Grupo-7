# Proyecto G7 - Integrante 3: Falcón Farfán Nehemías Luis

**Escenario:** Planificador de servicios de mudanza en Lima Metropolitana.  
Basado en `Propuesta_G7.docx` y el enunciado oficial del proyecto.

---

## Lo implementado

### C++ (Paradigma Imperativo)
- **4.1.6 Consultas básicas:**
  - `consultarPuntos`: lista formateada de los puntos registrados (código, nombre, tipo y zona).
  - `consultarConexiones`: muestra los tramos viales (habilitados y cerrados) conectados directa o inversamente a un punto.
  - `consultarMudanzasEstado`: filtra los servicios de mudanza por su estado (`pendiente`, `asignado`, `entregado`).
  - `consultarMudanzasCamion`: lista las mudanzas asignadas a una unidad de transporte específica.
- **4.1.7 Reporte general:**
  - `reporteGeneral`: vista consolidada de la red con el balance de puntos (depósitos vs. domicilios), estado de la red vial (vías habilitadas vs. cerradas), disponibilidad de la flota (disponibles vs. ocupados), desglose de pedidos por estado, volumen total en tránsito ($m^3$) y alertas de pedidos pendientes con prioridad alta.
- **Funcionalidad Adicional (C++):**
  - `cotizarMudanza`: recibe el código de un pedido pendiente, busca automáticamente la ruta habilitada más corta entre origen y destino, filtra los camiones disponibles con capacidad suficiente, calcula costo total, porcentaje de capacidad utilizada y precio por $m^3$, ordenando de menor a mayor y recomendando el camión más económico.

### Haskell (Paradigma Funcional)
- **4.2.3 Función de vecinos (`vecinos`):** función pura que devuelve los puntos conectados directamente a un nodo considerando doble sentido vial y únicamente vías con estado `"habilitada"`.
- **4.2.4 Rutas posibles (`rutasPosibles`):** algoritmo recursivo de búsqueda en profundidad (DFS) que genera todas las rutas posibles sin ciclos entre un origen y un destino.
- **4.2.5 Costo de ruta (`costoRuta`):** calcula la distancia y tiempo total de una ruta comprobando que cada tramo esté habilitado y multiplica los kilómetros acumulados por la tarifa por km del camión.
- **Funcionalidad Adicional (Haskell):**
  - Tipo de dato `CotizacionCamion` y función `cotizarServicio`: para un pedido pendiente, determina la ruta óptima disponible, compara los camiones disponibles aptos y devuelve la lista ordenada por costo estimado con el detalle analítico y la recomendación.

---

## Compilar y ejecutar (desde la raíz del proyecto)

### 1. Módulo en C++

#### Con MSVC (Visual Studio Developer PowerShell / CMD):
```powershell
cl.exe /std:c++17 /EHsc /I cpp\include cpp\src\main.cpp cpp\src\menu.cpp cpp\src\persistencia.cpp cpp\src\integrante2.cpp cpp\src\integrante3.cpp /Fe:mudanzas_app.exe
.\mudanzas_app.exe
```

#### Con GCC / MinGW (Linux / macOS / Git Bash):
```bash
g++ -std=c++17 -I cpp/include cpp/src/main.cpp cpp/src/menu.cpp cpp/src/persistencia.cpp cpp/src/integrante2.cpp cpp/src/integrante3.cpp -o mudanzas_app
./mudanzas_app
```

#### Opciones del menú correspondientes al Integrante 3:
- **8**: Consultar todos los puntos registrados.
- **9**: Consultar conexiones viales (ejemplo: ingresar `p01` o `p02`).
- **10**: Consultar mudanzas según su estado (ejemplo: ingresar `pendiente`).
- **11**: Consultar mudanzas por camión (ejemplo: ingresar `c01`).
- **12**: Generar reporte general estadístico del sistema.
- **14**: Cotización comparativa y recomendación de camión (ejemplo: ingresar `m001`).

---

### 2. Módulo en Haskell

Requiere GHC / GHCi instalado.

#### Ejecución interactiva en GHCi:
```bash
ghci -ihaskell haskell/Integrante3.hs
```

Dentro de la consola interactiva de Haskell:
```haskell
-- Cargar los datos desde la carpeta común 'datos/'
import LecturaCSV
Right dt <- leerDatos "datos"

-- 4.2.3 Probar vecinos de un punto
vecinos "p01" (conexiones dt)

-- 4.2.4 Probar rutas posibles sin ciclos entre dos puntos
rutasPosibles "p01" "p05" (conexiones dt)

-- 4.2.5 Probar costo de una ruta específica con un camión
let (cam02:_) = filter (\c -> codigoCamion c == "c02") (camiones dt)
costoRuta ["p01","p02","p04","p05"] cam02 (conexiones dt)

-- FA: Cotización comparativa y recomendación para un pedido
cotizarServicio "m001" dt
```

---

## Compatibilidad de los CSV

El sistema se apoya en los archivos compartidos en la carpeta `datos/`:
- `puntos.csv`: código, nombre, tipo, zona.
- `conexiones.csv`: origen, destino, distancia_km, tiempo_min, estado.
- `unidades.csv`: código, tipo, capacidad_m3, costo_km, estado.
- `pedidos.csv`: código, origen, destino, carga_m3, prioridad, estado, camion_asignado, fecha, franja.

**Nota:** Las funcionalidades del Integrante 3 son de naturaleza analítica y de consulta; **no modifican ni alteran la estructura de los archivos CSV base**, asegurando total compatibilidad con las funcionalidades de los demás integrantes.

---

## Archivos del Integrante 3

- Header C++: [`cpp/include/integrante3.hpp`](file:///c:/Users/nemi/Documents/Paradigmas/Paradigma-Grupo-7/cpp/include/integrante3.hpp)
- Implementación C++: [`cpp/src/integrante3.cpp`](file:///c:/Users/nemi/Documents/Paradigmas/Paradigma-Grupo-7/cpp/src/integrante3.cpp)
- Módulo Haskell: [`haskell/Integrante3.hs`](file:///c:/Users/nemi/Documents/Paradigmas/Paradigma-Grupo-7/haskell/Integrante3.hs)
