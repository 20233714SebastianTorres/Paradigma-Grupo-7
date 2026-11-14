#include "menu.hpp"
#include "persistencia.hpp"
#include <iostream>
#include "integrante2.hpp"
#include "integrante3.hpp"

int main() {
    const std::string carpeta = "datos";
    Datos datos;
    std::string error;
    if (!cargarDatos(carpeta, datos, error)) {
        std::cerr << "No se pudo cargar el sistema: " << error << '\n';
        return 1;
    }
    std::cout << "Carga inicial completada.\n";

    FuncionesGrupo funciones = crearFuncionesIntegrante2();
    conectarFuncionesIntegrante3(funciones);

    ejecutarMenu(datos, carpeta, funciones);
}
