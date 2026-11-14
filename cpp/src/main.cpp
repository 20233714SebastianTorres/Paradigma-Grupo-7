#include "menu.hpp"
#include "persistencia.hpp"
#include <iostream>
#include "integrante2.hpp"

// Para ejecutar desde la raiz del proyecto: ./mudanzas
int main() {
    const std::string carpeta = "datos";
    Datos datos;
    std::string error;
    if (!cargarDatos(carpeta, datos, error)) {
        std::cerr << "No se pudo cargar el sistema: " << error << '\n';
        return 1;
    }
    std::cout << "Carga inicial completada.\n";
    FuncionesGrupo funciones;  // El grupo conectara aqui sus funciones.
    FuncionesGrupo funciones = crearFuncionesIntegrante2();
    ejecutarMenu(datos, carpeta, funciones);
}
