# Integrante 4 - Haskell 4.2.6-4.2.7, Prolog 4.3.4-4.3.6 y funcionalidad adicional

Objetivo: implementar las funcionalidades base asignadas al integrante 4 y su funcionalidad adicional.
Rama: `feat/integrante4` | Entrega: un PR hacia `main` | Pruebas: `runghc` (Haskell) y `plunit` (SWI-Prolog).

## Specs

- S1 (4.2.6) "Reporte de pedidos: contar los pedidos por estado, calcular la carga total asignada por unidad y listar los pedidos pendientes ordenados por prioridad."
- S2 (4.2.7) "Resultado esperado: el programa debe mostrar un resumen como el siguiente:"
  ```
  Pedido: D002 (P01 -> P06)
  Rutas posibles: 3
  Costo de ruta P01 -> P03 -> P06 (moto): S/ 5.20
  Pedidos pendientes por prioridad: D004 (alta), D002 (media), D006 (baja)
  ```
- S3 (4.3.4) "Regla de ruta evitando un punto: obtener una ruta que no pase por un punto indicado. Ejemplo: ruta_evitando(p01, p06, p03, Ruta)."
- S4 (4.3.5) "Regla de atención de pedidos: determinar si una unidad puede atender un pedido (unidad disponible y capacidad suficiente). Ejemplo: puede_atender(u01, d002)."
- S5 (4.3.6) "Regla de pedidos inaccesibles: obtener los pedidos pendientes cuyo destino no es alcanzable desde su origen por las conexiones cerradas. Ejemplo: pedidos_inaccesibles(Pedidos)."
- S6 (FA, propuesta) "Simula el cierre de una vía e identifica los pedidos afectados, las rutas alternativas y el incremento de costos. Permite anticipar problemas en los traslados sin modificar los datos originales."
  - Prolog: "identifica los pedidos afectados y busca rutas alternativas mediante reglas y backtracking." Ejemplo: `analizar_cierre(p03, p05, Resultado).` -> "pedido m001 afectado, ruta alternativa disponible."
  - Haskell: "calcula las diferencias de distancia, tiempo y costo entre la ruta original y la alternativa." Ejemplo: `compararCostoRutas "c02" rutaOriginal rutaAlternativa` -> "costo original S/ 65.00, costo alternativo S/ 82.00, incremento S/ 17.00."
  - Supuesto: el tramo p03-p05 del ejemplo no existe en `conexiones.csv`; la demo usa p04-p05.
  - Supuesto: un pedido está "afectado" si su ruta más corta actual pasa por el tramo cerrado.
- S7 Integración: no romper lo existente (salida de `Main.hs` y consultas de `consolidacion.pl` idénticas a la línea base); eliminar la redefinición de `conectado/3`; README con comandos y consultas de ejemplo.

## Tasks

| ID | Specs | Ruta | Estado | Evidencia |
|---|---|---|---|---|
| T1 | S7 | inline | hecho | d5cba81; consolidacion sin cambios vs L3, sin warning |
| T2 | S3-S5 | inline | hecho | 166168e; RED (modulo inexistente) -> GREEN plunit |
| T3 | S6 (Prolog) | inline | hecho | 166168e; 19/19 plunit |
| T4 | S1-S2 | inline | hecho | RED (modulo inexistente) -> GREEN 16/16 |
| T5 | S6 (Haskell) | inline | hecho | 16/16; ghc -Wall sin warnings |
| T6 | S7 | inline | hecho | Main.hs: primeras 4 lineas iguales a L3; README_Integrante4.md |

Ruta inline: ninguna unidad supera ~150 líneas ni hay unidades paralelas independientes que justifiquen un writer.

## Log

- L1 (2026-11-14, usuario): "Quiero que hagas el discovery. Ahorita ya está hecho tanto el estudio 1 como la edición 3 y me falta a mí lo que sería el estudio 4. [...] Usa las buenas prácticas de ODE para poder llegar a un buen código y que sea fácil de explicar en la explicación también."
- L2 (2026-11-14, usuario): "arranca"
- L3 Línea base: `Main.hs` imprime "Ahorro: S/ 60.00"; `consolidable(c02,L)` -> [m001,m002], [m001,m006], [m002,m006]; `mejor_consolidacion` -> [m002,m006]-60.0; 22 hechos `conectado/3`.
- L4 Prolog: los primeros fixtures de plunit modificaban hechos en el modulo del test, no en user; se movieron a helpers en user.
- L5 Decision: no se crea prolog/main.pl; integrante4.pl ya carga consolidacion.pl e integrante2.pl.
- L6 Pendiente fuera de alcance: C++ no compila sin #include <cmath> (integrante 3); falta 'Unidad con mas pedidos' en 4.1.7; choque/3 cuenta pedidos entregados (integrante 2).
