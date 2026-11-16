# Paradigma-Grupo-7

**Sistema Planificador de Rutas y Logística** — Paradigmas de Programación, Universidad de Lima.

Escenario: planificador de servicios de mudanza en Lima Metropolitana. El sistema se implementa en tres paradigmas que comparten los mismos archivos CSV:

- **C++ (imperativo):** menú por consola para registrar, gestionar, consultar y guardar datos.
- **Haskell (funcional):** rutas, costos, reportes y cotizaciones.
- **Prolog (lógico):** conectividad, reglas de atención y consultas sobre la red.

## Integrantes y funcionalidades adicionales

| Integrante | Requisitos base | Funcionalidad adicional |
|---|---|---|
| 1 — Andre Sebastian Torres Hurtado | C++ 4.1.1, 4.1.2, 4.1.5 · Haskell 4.2.1, 4.2.2 | Consolidación de mudanzas pequeñas (`consolidacion.pl`, `Consolidacion.hs`) |
| 2 — Rado Delgado Jean Carlo de Jesus | C++ 4.1.3, 4.1.4 · Prolog 4.3.2, 4.3.3 | Agenda de servicios (`agendarMudanza`, `choque/3`, `camion_libre/3`) |
| 3 — Nehemias Falcon | C++ 4.1.6, 4.1.7 · Haskell 4.2.3, 4.2.4, 4.2.5 | Cotización comparativa (`cotizarMudanza`, `cotizarServicio`) |
| 4 — Walter Melendez | Haskell 4.2.6, 4.2.7 · Prolog 4.3.4, 4.3.5, 4.3.6 | Simulación de cierres viales (`impactoCierre`, `analizar_cierre/3`) |

Cada funcionalidad adicional está marcada en el código con un comentario `FA - Autor`.

## Estructura

```
cpp/        menú, persistencia CSV y funciones de los integrantes 2 y 3
haskell/    Tipos, LecturaCSV, Integrante3, Integrante4, Consolidacion, Main
prolog/     consolidacion.pl (carga de datos), integrante2.pl, integrante4.pl
datos/      puntos.csv, conexiones.csv, unidades.csv, pedidos.csv
pruebas/    pruebas automáticas de C++, Haskell y Prolog
```

El formato de cada CSV está descrito en [README_Integrante1.md](README_Integrante1.md#compatibilidad-de-los-csv).

## Cómo ejecutar (desde la carpeta raíz)

Requiere g++ con C++17, GHC y SWI-Prolog.

```bash
# C++
g++ -std=c++17 -I cpp/include cpp/src/*.cpp -o mudanzas
./mudanzas

# Haskell
runghc -ihaskell haskell/Main.hs

# Prolog
swipl prolog/integrante4.pl
?- cargar_datos.
```

En Windows (MinGW), reemplazar `cpp/src/*.cpp` por la lista de archivos: `cpp/src/main.cpp cpp/src/menu.cpp cpp/src/persistencia.cpp cpp/src/integrante2.cpp cpp/src/integrante3.cpp`.

## Pruebas

| Comando | Qué cubre |
|---|---|
| `g++ -std=c++17 -I cpp/include pruebas/test_persistencia.cpp cpp/src/persistencia.cpp -o prueba_csv && ./prueba_csv` | Lectura y escritura de los CSV |
| `g++ -std=c++17 -I cpp/include pruebas/test_validaciones.cpp cpp/src/integrante2.cpp cpp/src/integrante3.cpp -o prueba_validaciones && ./prueba_validaciones` | Validaciones de registro, estados, agenda y reporte general |
| `runghc -ihaskell pruebas/TestIntegrante4.hs` | Haskell 4.2.6, 4.2.7 y cierres viales |
| `swipl -g run_tests -t halt pruebas/test_integrante4.pl` | Prolog 4.3.4–4.3.6 y cierres viales |
| `swipl -g run_tests -t halt pruebas/test_consolidacion_agenda.pl` | Consolidación y agenda en Prolog |

Más detalle de cada parte en [README_Integrante1.md](README_Integrante1.md) y [README_Integrante4.md](README_Integrante4.md).

## Fechas

Entrega final: **16 de noviembre de 2026**. Exposición: **17 de noviembre de 2026**.
