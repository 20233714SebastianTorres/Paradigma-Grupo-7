# Proyecto G7 - Integrante 4

**Autor:** Andre Sebastian Torres Hurtado
**Escenario:** Planificador de servicios de mudanza en Lima Metropolitana.

## Lo implementado

| Requisito | Lenguaje | Dónde |
|---|---|---|
| 4.2.6 Reporte de pedidos | Haskell | `contarPorEstado`, `cargaPorCamion`, `pendientesPorPrioridad`, `reportePedidos` en `haskell/Integrante4.hs` |
| 4.2.7 Resultado esperado | Haskell | `resumenPedido` en `haskell/Integrante4.hs` |
| 4.3.4 Ruta evitando un punto | Prolog | `ruta_evitando/4` en `prolog/integrante4.pl` |
| 4.3.5 Atención de pedidos | Prolog | `puede_atender/2` |
| 4.3.6 Pedidos inaccesibles | Prolog | `pedidos_inaccesibles/1` |
| **FA** Simulación de cierres viales | Prolog + Haskell | `analizar_cierre/3`; `simularCierre`, `impactoCierre`, `compararCostoRutas` |

Se reutiliza el trabajo del grupo sin duplicarlo: `cargar_datos/0` (lectura de los CSV), `conectado/3` y `alcanzable/2` en Prolog; `rutasPosibles`, `costoRuta`, `distanciaYTiempoRuta` y `rutaMasCorta` del integrante 3 en Haskell.

## Funcionalidad adicional: simulación de cierres viales

Simula el cierre de un tramo y muestra qué pedidos pendientes se ven afectados, cuál es su ruta alternativa y cuánto aumentan la distancia, el tiempo y el costo. **Los datos originales no se modifican.**

- Un pedido está **afectado** si su ruta más corta actual pasa por el tramo cerrado.
- **Prolog** (`analizar_cierre/3`): el tramo cerrado se pasa como parámetro (`sin_tramo(A, B)`) a la regla que recorre la red. No se usa `assert` ni `retract`, así que la base de conocimiento queda intacta. El backtracking encuentra todas las rutas alternativas y se elige la más corta.
- **Haskell** (`impactoCierre`): `simularCierre` devuelve una **copia** de los datos con el tramo cerrado. Como los datos son inmutables, el original nunca cambia. `compararCostoRutas` calcula las diferencias.
- Si el tramo no existe o ya está cerrado: Prolog falla y Haskell devuelve `Left` con el motivo.

> En la propuesta, el ejemplo usa el tramo p03-p05, pero ese tramo no existe en `datos/conexiones.csv`. La demo usa p04-p05.

## Ejecutar (desde la carpeta raíz)

### Haskell

```bash
runghc -ihaskell haskell/Main.hs
```

Salida de la parte del integrante 4:

```
--- 4.2.6 Reporte de pedidos ---
Pedidos por estado: pendiente: 3 | asignado: 1 | entregado: 2
Carga asignada a c01: 10.0 m3
Carga asignada a c02: 0.0 m3
Carga asignada a c03: 0.0 m3
Pendientes por prioridad: m001 (alta), m006 (alta), m002 (media)
--- 4.2.7 Resultado esperado ---
Pedido: m002 (p02 -> p06)
Rutas posibles: 9
Costo de ruta p02 -> p03 -> p04 -> p08 -> p06 (camion_mediano): S/ 78.00
Pedidos pendientes por prioridad: m001 (alta), m006 (alta), m002 (media)
--- FA: simulacion del cierre p04 - p05 (camion c02) ---
Pedido m001 afectado: p01 -> p03 -> p04 -> p05
  Alternativa: p01 -> p03 -> p04 -> p08 -> p06 -> p05
  Distancia: 21.0 km -> 26.5 km (+5.5 km)
  Tiempo: 62 min -> 79 min (+17 min)
  Costo: S/ 84.00 -> S/ 106.00 (incremento S/ 22.00)
```

### Prolog

```bash
swipl prolog/integrante4.pl
```

```prolog
?- cargar_datos.
?- ruta_evitando(p01, p06, p03, Ruta).
Ruta = [p01, p02, p04, p05, p06] ;      % ";" muestra más rutas (backtracking)
?- puede_atender(c02, m001).
true.
?- puede_atender(Camion, m001).
Camion = c02 ; Camion = c03.
?- pedidos_inaccesibles(Pedidos).
Pedidos = [].                           % con los datos actuales todo destino es alcanzable
?- analizar_cierre(p04, p05, Resultado).
Resultado = [afectado(m001, [p01,p03,p04,p05], 21,
             alternativa([p01,p03,p04,p08,p06,p05], 26.5))].
```

## Pruebas

```bash
runghc -ihaskell pruebas/TestIntegrante4.hs          # 16 pruebas
swipl -g run_tests -t halt pruebas/test_integrante4.pl  # 19 pruebas (plunit)
```
