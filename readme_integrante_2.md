# Proyecto G7 - Integrante 2

**Autor:** Rado Delgado Jean Carlo de Jesús
**Escenario:** Planificador de servicios de mudanza en Lima Metropolitana.

## Lo implementado

| Requisito | Lenguaje | Dónde |
|---|---|---|
| 4.1.3 Registro con validaciones | C++ | `registrarPunto`, `registrarConexion`, `registrarCamion`, `registrarMudanza` en `cpp/src/integrante2.cpp` |
| 4.1.4 Gestión de estados | C++ | `gestionarConexion`, `asignarMudanza`, `entregarMudanza` en `cpp/src/integrante2.cpp` |
| 4.3.1 Lectura de CSV | Prolog | `cargar_datos/0` (lectura con `csv_read_file` y `assertz`) | 
| 4.3.2 Conexión directa | Prolog | `conectado/3` en `prolog/integrante2.pl` |
| 4.3.3 Alcanzabilidad | Prolog | `alcanzable/2`, `alcanzable_aux/3` en `prolog/integrante2.pl` |
| **FA** Agenda de servicios | C++ + Prolog | Opción 13 en el menú de c++; `choque/3`, `camion_libre/3` en `prolog/integrante2.pl` |

## Funcionalidad adicional: agenda de servicios

Asigna una **fecha** (`AAAA-MM-DD`) y una **franja horaria** (`manana` o `tarde`) a cada servicio de mudanza, evitando que un mismo camión sea programado en dos mudanzas que chocan en el mismo día y turno.

- Un camión tiene un **choque** si ya está asignado a un pedido cuya fecha y franja coinciden con los que se intentan agendar.
- **C++** (Opción 13 del menú): solicita el código de mudanza, la fecha y la franja. Recorre los pedidos registrados y bloquea la operación si detecta un choque con el camión asignado a esa mudanza, protegiendo los datos antes de guardarlos.
- **Prolog** (`choque/3` y `camion_libre/3`): `choque/3` comprueba en los hechos `mudanza/9` si un camión ya figura en un servicio para esa fecha y franja. `camion_libre/3` verifica que no haya choque utilizando `not/1`.
- **Contrato de datos:** se añaden las columnas `fecha` y `franja` al final de `datos/pedidos.csv` sin alterar la estructura original para el resto del equipo.

## Ejecutar (desde la carpeta raíz)

### C++

```bash
g++ -std=c++17 -I cpp/include cpp/src/main.cpp cpp/src/menu.cpp cpp/src/persistencia.cpp cpp/src/integrante2.cpp cpp/src/integrante3.cpp -o mudanzas.exe
.\mudanzas.exe
```

Salida de la parte del integrante 2 (basada en la guía de comandos):

```text
--- 4.1.3 Registro con validaciones ---
Ingrese opcion: 1
Codigo del punto: p01
Error: El codigo ya existe.
--- 4.1.4 Gestion de estados ---
Ingrese opcion: 6
Codigo de mudanza: m007
Codigo del camion: c02
Error: El camion no tiene capacidad suficiente.
--- FA: Agenda de servicios ---
Ingrese opcion: 13
Codigo de mudanza: m007
Nueva Fecha (AAAA-MM-DD): 2026-11-25
Nueva Franja horaria (manana/tarde): tarde
Mudanza agendada correctamente.
```

### Prolog

```bash
swipl -s prolog/consolidacion.pl -s prolog/integrante2.pl
```

```prolog
?- cargar_datos.
true.
?- conectado(p01, p03, D).
D = 6 .
?- alcanzable(p01, p06).
true .
?- choque(c03, '2026-11-25', tarde).
true.                                   % El camión c03 ya está ocupado (tiene a m007)
?- camion_libre(c03, '2026-11-25', manana).
true.
```

