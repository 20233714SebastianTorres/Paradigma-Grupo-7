#include "persistencia.hpp"
#include "integrante3.hpp"
#include <cassert>
#include <iostream>

int main() {
    Datos datos;
    std::string error;
    assert(cargarDatos("datos", datos, error));

    // 1. Verificacion de que los datos requeridos existen
    assert(datos.puntos.size() >= 8);
    assert(datos.conexiones.size() >= 12);
    assert(datos.camiones.size() >= 3);
    assert(datos.mudanzas.size() >= 6);

    // 2. Conectar las funciones a traves del struct FuncionesGrupo
    FuncionesGrupo fg;
    conectarFuncionesIntegrante3(fg);

    // 3. Comprobar que los slots fueron conectados
    assert(fg.consultarPuntos != nullptr);
    assert(fg.consultarConexiones != nullptr);
    assert(fg.consultarMudanzasEstado != nullptr);
    assert(fg.consultarMudanzasCamion != nullptr);
    assert(fg.reporteGeneral != nullptr);
    assert(fg.cotizarMudanza != nullptr);

    // 4. Ejecutar reporteGeneral y consultarPuntos sin errores
    assert(fg.reporteGeneral(datos) == false);
    assert(fg.consultarPuntos(datos) == false);

    std::cout << "\nOK: Pruebas automatizadas de consultas y reporte (Integrante 3) pasaron exitosamente.\n";
    return 0;
}
