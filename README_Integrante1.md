# Proyecto G7 - Integrante 1

**Escenario:** Planificador de servicios de mudanza en Lima Metropolitana.
Basado en `Propuesta_G7.docx` y el enunciado del proyecto.

## Lo implementado

- C++ **4.1.1**: menu por consola que admite registrar, gestionar y consultar mediante acciones integrables.
- C++ **4.1.2**: carga inicial de cuatro CSV. Si no existen, comienza con estructuras vacias.
- C++ **4.1.5**: escritura de cuatro CSV; se guarda tras cada modificacion efectuada por callbacks del grupo.
- Haskell **4.2.1**: lectura de archivos CSV, con errores de formato.
- Haskell **4.2.2**: tipos Punto, Conexion, Camion, Mudanza y Datos.
- **Funcionalidad adicional**: Prolog `consolidable/2`, `mejor_consolidacion/3`; Haskell `ahorroConsolidacion`.

## Compilar y ejecutar (desde la carpeta raiz)

Requiere g++ 17, GHC/RunHaskell y SWI-Prolog instalados.

### C++

Linux/macOS:

```bash
g++ -std=c++17 -I cpp/include cpp/src/*.cpp -o mudanzas
./mudanzas
```

Windows (MinGW):

```powershell
g++ -std=c++17 -I cpp/include cpp/src/main.cpp cpp/src/menu.cpp cpp/src/persistencia.cpp cpp/src/integrante2.cpp cpp/src/integrante3.cpp -o mudanzas.exe
.\mudanzas.exe
```

En el menu, pulse **17** para verificar los CSV cargados, **15** para recargar y **16** para guardar. Las opciones 1-14 ejecutan las funciones de los integrantes 2 y 3, conectadas en `main.cpp` (no se atribuyen sus entregables al integrante 1).

### Haskell

```bash
runghc -ihaskell haskell/Main.hs
```

La consola debe mostrar la carga de los 4 CSV y un ejemplo de calculo de ahorro. En codigo, se puede usar:

```haskell
ahorroConsolidacion ["m002", "m006"] "c02" datos
```

### Prolog

```bash
swipl -s prolog/consolidacion.pl
```

En la consola Prolog:

```prolog
?- cargar_datos.
?- consolidable(c02, L).
?- mejor_consolidacion(c02, L, Ahorro).
?- ahorro_grupo(c02, [m002,m006], Ahorro).
```

## Compatibilidad de los CSV

Los archivos estan dentro de `datos/` y se leen al ejecutar desde la raiz del proyecto. **No tienen encabezados**, pero los lectores los admiten opcionalmente.

- `puntos.csv`: codigo, nombre, tipo, zona
- `conexiones.csv`: origen, destino, distancia_km, tiempo_min, estado
- `unidades.csv`: codigo, tipo, capacidad_m3, costo_km, estado
- `pedidos.csv`: codigo, origen, destino, carga_m3, prioridad, estado, camion_asignado, fecha, franja

`pedidos.csv` admite tambien la version antigua de 7 campos; al guardarse se escribe la version de 9 campos por compatibilidad con la **Agenda de servicios** del integrante 2. Todos los codigos y estados de CSV usan minusculas (`c02`, `pendiente`, `disponible`, `habilitada`). Evitar renombrarlos sin actualizar los tres lenguajes.

## Datos de demostracion

8 puntos, 12 conexiones (1 cerrada), 3 camiones, 6 servicios con estados pendiente/asignado/entregado. **Las distancias son datos de prueba ilustrativos, no medidas viales verificadas.**

## Como unir con el trabajo de tus compañeros

1. Compartir `cpp/include/modelos.hpp` como esquema comun para evitar structs diferentes.
2. Integrante 2 crea operaciones de registro y gestion que reciban `Datos&`, retornen `true` solo cuando modifican datos y se conecten a `FuncionesGrupo` en `main.cpp`.
3. Integrante 3 crea operaciones de consulta sobre `Datos&` y se conectan de igual manera (retornan `false`).
4. Integrante 2 incorpora la agenda conservando `fecha` y `franja` en `Mudanza`.
5. Haskell de los integrantes 3 y 4 puede importar `Tipos` y `LecturaCSV` sin duplicar lectores.
6. Prolog adicional puede consultarse desde su propio archivo o integrarse con el modulo Prolog del integrante 2; evitar definir dos veces `conectado/3` o `cargar_datos/0` sin coordinacion.

## Suposiciones del ahorro

- Todos los pedidos consolidados deben estar **pendientes**, salir de la **misma zona** y caber juntos en un camion **disponible**.
- Solo se usan vias **habilitadas**, en ambos sentidos.
- Por separado: suma de las distancias minimas entre origen y destino, multiplicada por la tarifa/km del camion seleccionado.
- Consolidados: se visitan todos los puntos de recojo y luego todos los destinos, escogiendo el orden mas corto por kilometros; tarifa constante por km.
- No se contabiliza el viaje desde/hacia un deposito, costos fijos, carga/descarga ni horarios (no exigidos en el ejemplo individual). Documentarlo en el informe.
- `mejor_consolidacion/3` elige el grupo con el **mayor ahorro positivo** calculado bajo estas mismas suposiciones.

## Pruebas

```bash
g++ -std=c++17 -I cpp/include pruebas/test_persistencia.cpp cpp/src/persistencia.cpp -o prueba_csv
./prueba_csv
```

Esta prueba comprueba cantidad de registros, guardado y relectura de fecha/franja.

Validaciones de registro (4.1.3), gestion de estados (4.1.4), agenda (fecha y franja) y reporte general (4.1.7):

```bash
g++ -std=c++17 -I cpp/include pruebas/test_validaciones.cpp cpp/src/integrante2.cpp cpp/src/integrante3.cpp -o prueba_validaciones
./prueba_validaciones
```

Consolidacion (integrante 1) y agenda (integrante 2) en Prolog:

```bash
swipl -g run_tests -t halt pruebas/test_consolidacion_agenda.pl
```

Para validar los demas modulos, ejecutarlos con GHC y SWI-Prolog instalados.

## Fecha del proyecto

Entrega final del grupo: **16 de noviembre de 2026**. Exposicion: **17 de noviembre de 2026**.
