#pragma once
#include "modelos.hpp"
#include <functional>
#include <string>

// 4.1.1: menu central. Los callbacks los implementan los otros integrantes.
// En las opciones de modificacion, devolver true SOLAMENTE si hubo un cambio.
using AccionMenu = std::function<bool(Datos&)>;
struct FuncionesGrupo {
    AccionMenu registrarPunto;          // Int. 2 (4.1.3)
    AccionMenu registrarConexion;       // Int. 2 (4.1.3)
    AccionMenu registrarCamion;         // Int. 2 (4.1.3)
    AccionMenu registrarMudanza;        // Int. 2 (4.1.3)
    AccionMenu gestionarConexion;       // Int. 2 (4.1.4)
    AccionMenu asignarMudanza;          // Int. 2 (4.1.4)
    AccionMenu entregarMudanza;         // Int. 2 (4.1.4)
    AccionMenu consultarPuntos;         // Int. 3 (4.1.6)
    AccionMenu consultarConexiones;     // Int. 3 (4.1.6)
    AccionMenu consultarMudanzasEstado; // Int. 3 (4.1.6)
    AccionMenu consultarMudanzasCamion; // Int. 3 (4.1.6)
    AccionMenu reporteGeneral;          // Int. 3 (4.1.7)
    AccionMenu agendarMudanza;          // Int. 2 (adicional)
    AccionMenu cotizarMudanza;          // Int. 3 (adicional)
};

void ejecutarMenu(Datos& datos, const std::string& carpeta,
                  const FuncionesGrupo& funciones);
