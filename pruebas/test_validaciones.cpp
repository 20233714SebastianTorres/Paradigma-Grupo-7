// Tests for C++ validations (4.1.3), state management (4.1.4) and report (4.1.7).
// Build and run from the project root:
//   g++ -std=c++17 -I cpp/include pruebas/test_validaciones.cpp cpp/src/integrante2.cpp cpp/src/integrante3.cpp -o prueba_validaciones
//   ./prueba_validaciones
#include "integrante2.hpp"
#include "integrante3.hpp"
#include <iostream>
#include <sstream>
#include <string>

namespace {
int fallas = 0;

void prueba(const std::string& nombre, bool ok) {
    std::cout << (ok ? "[OK]    " : "[FALLA] ") << nombre << '\n';
    if (!ok) ++fallas;
}

// Runs a menu action feeding `entrada` as keyboard input; captures its output.
bool ejecutar(const AccionMenu& accion, Datos& datos, const std::string& entrada,
              std::string* salida = nullptr) {
    std::istringstream in(entrada);
    std::ostringstream out;
    auto* cinViejo = std::cin.rdbuf(in.rdbuf());
    auto* coutViejo = std::cout.rdbuf(out.rdbuf());
    bool r = accion(datos);
    std::cin.rdbuf(cinViejo);
    std::cout.rdbuf(coutViejo);
    std::cin.clear();
    if (salida) *salida = out.str();
    return r;
}

Datos base() {
    Datos d;
    d.puntos = {{"p01", "A", "deposito", "Z1"}, {"p02", "B", "domicilio", "Z1"},
                {"p03", "C", "domicilio", "Z2"}};
    d.conexiones = {{"p01", "p02", 5, 10, "habilitada"}};
    d.camiones = {{"c01", "mediano", 10, 3, "disponible"}, {"c02", "grande", 20, 5, "disponible"}};
    d.mudanzas = {{"m01", "p01", "p02", 4, "alta", "pendiente", "", "", ""},
                  {"m02", "p01", "p03", 3, "media", "pendiente", "", "", ""},
                  {"m03", "p02", "p03", 2, "baja", "pendiente", "", "", ""}};
    return d;
}
} // namespace

int main() {
    FuncionesGrupo f = crearFuncionesIntegrante2();
    conectarFuncionesIntegrante3(f);

    // --- 4.1.3 numeric validations ---
    {
        Datos d = base();
        prueba("conexion con distancia negativa se rechaza",
               !ejecutar(f.registrarConexion, d, "p02 p03 -5 10 habilitada\n") && d.conexiones.size() == 1);
        prueba("conexion con tiempo cero se rechaza",
               !ejecutar(f.registrarConexion, d, "p02 p03 4 0 habilitada\n") && d.conexiones.size() == 1);
        prueba("conexion con distancia no numerica se rechaza y no rompe la entrada",
               !ejecutar(f.registrarConexion, d, "p02 p03 abc 10 habilitada\n") && d.conexiones.size() == 1);
        prueba("conexion con estado invalido se rechaza",
               !ejecutar(f.registrarConexion, d, "p02 p03 4 10 abierta\n") && d.conexiones.size() == 1);
        prueba("conexion de un punto consigo mismo se rechaza",
               !ejecutar(f.registrarConexion, d, "p02 p02\n") && d.conexiones.size() == 1);
        prueba("conexion valida se registra (previous behavior)",
               ejecutar(f.registrarConexion, d, "p02 p03 4 10 habilitada\n") && d.conexiones.size() == 2);
    }
    {
        Datos d = base();
        prueba("tramo duplicado en sentido inverso se rechaza",
               !ejecutar(f.registrarConexion, d, "p02 p01\n") && d.conexiones.size() == 1);
        prueba("tramo duplicado en el mismo sentido se rechaza (previous behavior)",
               !ejecutar(f.registrarConexion, d, "p01 p02\n") && d.conexiones.size() == 1);
    }
    {
        Datos d = base();
        prueba("mudanza con carga cero se rechaza",
               !ejecutar(f.registrarMudanza, d, "m09 p01 p02 0 alta\n") && d.mudanzas.size() == 3);
        prueba("mudanza con prioridad invalida se rechaza",
               !ejecutar(f.registrarMudanza, d, "m09 p01 p02 5 urgente\n") && d.mudanzas.size() == 3);
        prueba("mudanza valida se registra pendiente (previous behavior)",
               ejecutar(f.registrarMudanza, d, "m09 p01 p02 5 alta\n") && d.mudanzas.size() == 4 &&
               d.mudanzas.back().estado == "pendiente");
        prueba("camion con capacidad negativa se rechaza",
               !ejecutar(f.registrarCamion, d, "c09 mediano -3 2 disponible\n") && d.camiones.size() == 2);
        prueba("camion con estado invalido se rechaza",
               !ejecutar(f.registrarCamion, d, "c09 mediano 3 2 libre\n") && d.camiones.size() == 2);
        prueba("camion valido se registra (previous behavior)",
               ejecutar(f.registrarCamion, d, "c09 mediano 3 2 disponible\n") && d.camiones.size() == 3);
    }

    // --- 4.1.4 state management ---
    {
        Datos d = base();
        prueba("cerrar conexion indicando el sentido inverso",
               ejecutar(f.gestionarConexion, d, "p02 p01\n") && d.conexiones[0].estado == "cerrada");
        prueba("habilitar conexion en el sentido registrado (previous behavior)",
               ejecutar(f.gestionarConexion, d, "p01 p02\n") && d.conexiones[0].estado == "habilitada");
    }
    {
        Datos d = base();
        d.camiones[0].estado = "ocupada";
        d.mudanzas[0].estado = "asignado"; d.mudanzas[0].camionAsignado = "c01";
        d.mudanzas[1].estado = "asignado"; d.mudanzas[1].camionAsignado = "c01";
        ejecutar(f.entregarMudanza, d, "m01\n");
        prueba("entregar no libera un camion que aun tiene otra mudanza asignada",
               d.mudanzas[0].estado == "entregado" && d.camiones[0].estado == "ocupada");
        ejecutar(f.entregarMudanza, d, "m02\n");
        prueba("entregar la ultima mudanza libera el camion (previous behavior)",
               d.mudanzas[1].estado == "entregado" && d.camiones[0].estado == "disponible");
    }

    // --- additional feature (integrante 2): schedule ---
    {
        Datos d = base();
        prueba("agenda con fecha inexistente se rechaza",
               !ejecutar(f.agendarMudanza, d, "m01 2026-02-30 10:00-12:00\n") && d.mudanzas[0].fecha.empty());
        prueba("agenda con fecha mal formada se rechaza",
               !ejecutar(f.agendarMudanza, d, "m01 15/11/2026 10:00-12:00\n") && d.mudanzas[0].fecha.empty());
        prueba("agenda con franja invertida se rechaza",
               !ejecutar(f.agendarMudanza, d, "m01 2026-11-20 12:00-10:00\n") && d.mudanzas[0].franja.empty());
        prueba("agenda con hora invalida se rechaza",
               !ejecutar(f.agendarMudanza, d, "m01 2026-11-20 25:00-26:00\n") && d.mudanzas[0].franja.empty());
        d.mudanzas[2].estado = "entregado";
        prueba("agenda de una mudanza entregada se rechaza",
               !ejecutar(f.agendarMudanza, d, "m03 2026-11-20 10:00-12:00\n") && d.mudanzas[2].fecha.empty());
        prueba("agenda en 29 de febrero de un bisiesto se acepta",
               ejecutar(f.agendarMudanza, d, "m02 2028-02-29 08:00-09:30\n") && d.mudanzas[1].fecha == "2028-02-29");
        prueba("agenda valida guarda fecha y franja (previous behavior)",
               ejecutar(f.agendarMudanza, d, "m01 2026-11-20 10:00-12:00\n") &&
               d.mudanzas[0].fecha == "2026-11-20" && d.mudanzas[0].franja == "10:00-12:00");
        prueba("agenda de mudanza inexistente se rechaza (previous behavior)",
               !ejecutar(f.agendarMudanza, d, "m99\n"));
    }

    // --- 4.1.7 general report ---
    {
        Datos d = base();
        d.mudanzas[0].estado = "asignado"; d.mudanzas[0].camionAsignado = "c02";
        d.mudanzas[1].estado = "asignado"; d.mudanzas[1].camionAsignado = "c02";
        d.mudanzas[2].estado = "asignado"; d.mudanzas[2].camionAsignado = "c01";
        std::string salida;
        ejecutar(f.reporteGeneral, d, "", &salida);
        prueba("reporte muestra la unidad con mas pedidos",
               salida.find("Unidad con mas pedidos: c02 (2 pedidos)") != std::string::npos);
        prueba("reporte conserva los conteos (previous behavior)",
               salida.find("Asignadas  : 3") != std::string::npos);
        Datos vacio;
        ejecutar(f.reporteGeneral, vacio, "", &salida);
        prueba("reporte sin pedidos asignados indica que no hay unidad",
               salida.find("Unidad con mas pedidos: ninguna") != std::string::npos);
    }

    if (fallas) { std::cout << fallas << " prueba(s) fallaron.\n"; return 1; }
    std::cout << "Todas las pruebas pasaron.\n";
}
