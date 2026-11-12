// FA - Integrante 1: menu principal (funcionalidad 4.1.1).
#include "menu.hpp"
#include "persistencia.hpp"
#include <iostream>
#include <sstream>

namespace {
int pedirOpcion() {
    std::cout << "Seleccione una opcion: ";
    std::string linea, extra;
    if (!std::getline(std::cin, linea)) return 0;
    std::istringstream entrada(linea);
    int opcion;
    if (!(entrada >> opcion) || (entrada >> extra)) return -1;
    return opcion;
}

void imprimirMenu() {
    std::cout << "\n=== PLANIFICADOR DE MUDANZAS - LIMA ===\n"
              << " 1. Registrar punto\n"
              << " 2. Registrar conexion\n"
              << " 3. Registrar camion\n"
              << " 4. Registrar mudanza\n"
              << " 5. Cerrar o habilitar conexion\n"
              << " 6. Asignar mudanza a camion\n"
              << " 7. Marcar mudanza entregada\n"
              << " 8. Consultar puntos\n"
              << " 9. Consultar conexiones de un punto\n"
              << "10. Consultar mudanzas por estado\n"
              << "11. Consultar mudanzas por camion\n"
              << "12. Reporte general\n"
              << "13. Agendar servicio (adicional integrante 2)\n"
              << "14. Cotizar servicio (adicional integrante 3)\n"
              << "15. Recargar archivos CSV\n"
              << "16. Guardar archivos CSV\n"
              << "17. Ver cantidad de registros cargados\n"
              << " 0. Salir\n";
}

bool accionar(const AccionMenu& accion, Datos& datos) {
    if (!accion) {
        std::cout << "Pendiente de integrar: esta funcion pertenece a otro integrante.\n";
        return false;
    }
    return accion(datos);
}
}

void ejecutarMenu(Datos& datos, const std::string& carpeta,
                  const FuncionesGrupo& funciones) {
    for (;;) {
        imprimirMenu();
        int opcion = pedirOpcion();
        if (opcion == 0) { std::cout << "Hasta luego.\n"; return; }
        std::string error;
        bool huboCambio = false;
        switch (opcion) {
            case 1: huboCambio = accionar(funciones.registrarPunto, datos); break;
            case 2: huboCambio = accionar(funciones.registrarConexion, datos); break;
            case 3: huboCambio = accionar(funciones.registrarCamion, datos); break;
            case 4: huboCambio = accionar(funciones.registrarMudanza, datos); break;
            case 5: huboCambio = accionar(funciones.gestionarConexion, datos); break;
            case 6: huboCambio = accionar(funciones.asignarMudanza, datos); break;
            case 7: huboCambio = accionar(funciones.entregarMudanza, datos); break;
            case 8: accionar(funciones.consultarPuntos, datos); break;
            case 9: accionar(funciones.consultarConexiones, datos); break;
            case 10: accionar(funciones.consultarMudanzasEstado, datos); break;
            case 11: accionar(funciones.consultarMudanzasCamion, datos); break;
            case 12: accionar(funciones.reporteGeneral, datos); break;
            case 13: huboCambio = accionar(funciones.agendarMudanza, datos); break;
            case 14: accionar(funciones.cotizarMudanza, datos); break;
            case 15:
                if (cargarDatos(carpeta, datos, error)) std::cout << "CSV recargados.\n";
                else std::cerr << "Error: " << error << '\n';
                break;
            case 16:
                if (guardarDatos(carpeta, datos, error)) std::cout << "CSV guardados.\n";
                else std::cerr << "Error: " << error << '\n';
                break;
            case 17:
                std::cout << "Puntos: " << datos.puntos.size()
                          << " | Conexiones: " << datos.conexiones.size()
                          << " | Camiones: " << datos.camiones.size()
                          << " | Mudanzas: " << datos.mudanzas.size() << "\n";
                break;
            default: std::cout << "Opcion no valida.\n"; break;
        }
        if (huboCambio) {
            if (!guardarDatos(carpeta, datos, error)) std::cerr << "Error al guardar: " << error << '\n';
            else std::cout << "Cambios guardados en CSV.\n";
        }
    }
}
