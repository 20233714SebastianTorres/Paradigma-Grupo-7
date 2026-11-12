#include "persistencia.hpp"
#include <cassert>
#include <filesystem>
#include <iostream>
int main() {
    Datos datos;
    std::string error;
    assert(cargarDatos("datos", datos, error));
    assert(datos.puntos.size() == 8);
    assert(datos.conexiones.size() == 12);
    assert(datos.camiones.size() == 3);
    assert(datos.mudanzas.size() == 6);
    assert(datos.mudanzas[2].fecha == "2026-11-20");
    assert(datos.mudanzas[2].franja == "manana");
    const auto dir = std::filesystem::temp_directory_path() / "g7_integrante1_csv_prueba";
    std::filesystem::remove_all(dir);
    assert(guardarDatos(dir.string(), datos, error));
    Datos leidos;
    assert(cargarDatos(dir.string(), leidos, error));
    assert(leidos.puntos.size() == 8);
    assert(leidos.mudanzas[2].fecha == "2026-11-20");
    assert(leidos.mudanzas[2].camionAsignado == "c01");
    std::filesystem::remove_all(dir);
    std::cout << "OK: carga, guardado y preservacion de fecha/franja\n";
}
